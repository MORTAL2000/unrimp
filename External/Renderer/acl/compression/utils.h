#pragma once

////////////////////////////////////////////////////////////////////////////////
// The MIT License (MIT)
//
// Copyright (c) 2018 Nicholas Frechette & Animation Compression Library contributors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include "acl/core/impl/compiler_utils.h"
#include "acl/core/compressed_clip.h"
#include "acl/core/iallocator.h"
#include "acl/compression/compression_settings.h"
#include "acl/compression/skeleton.h"
#include "acl/compression/animation_clip.h"
#include "acl/decompression/default_output_writer.h"

#include <cstdint>
#include <cstring>

ACL_IMPL_FILE_PRAGMA_PUSH

namespace acl
{
	struct BoneError
	{
		BoneError() : index(k_invalid_bone_index), error(0.0F), sample_time(0.0F) {}

		uint16_t index;
		float error;
		float sample_time;
	};

	template<class DecompressionContextType>
	inline BoneError calculate_compressed_clip_error(IAllocator& allocator,
		const AnimationClip& clip, const itransform_error_metric& error_metric, DecompressionContextType& context)
	{
		const uint32_t num_samples = clip.get_num_samples();
		if (num_samples == 0)
			return BoneError();

		const uint16_t num_bones = clip.get_num_bones();
		if (num_bones == 0)
			return BoneError();

		const float clip_duration = clip.get_duration();
		const float sample_rate = clip.get_sample_rate();
		const RigidSkeleton& skeleton = clip.get_skeleton();

		// Always calculate the error with scale, slower but we don't need to know if we have scale or not
		const bool has_scale = true;

		uint16_t num_output_bones = 0;
		uint16_t* output_bone_mapping = create_output_bone_mapping(allocator, clip, num_output_bones);

		const AnimationClip* additive_base_clip = clip.get_additive_base();
		const uint32_t additive_num_samples = additive_base_clip != nullptr ? additive_base_clip->get_num_samples() : 0;
		const float additive_duration = additive_base_clip != nullptr ? additive_base_clip->get_duration() : 0.0F;

		rtm::qvvf* raw_local_pose = allocate_type_array<rtm::qvvf>(allocator, num_bones);
		rtm::qvvf* base_local_pose = additive_base_clip != nullptr ? allocate_type_array<rtm::qvvf>(allocator, num_bones) : nullptr;
		rtm::qvvf* lossy_local_pose = allocate_type_array<rtm::qvvf>(allocator, num_output_bones);
		rtm::qvvf* lossy_remapped_local_pose = allocate_type_array<rtm::qvvf>(allocator, num_bones);

		const size_t transform_size = error_metric.get_transform_size(has_scale);
		const bool needs_conversion = error_metric.needs_conversion(has_scale);
		uint8_t* raw_local_pose_converted = nullptr;
		uint8_t* base_local_pose_converted = nullptr;
		uint8_t* lossy_local_pose_converted = nullptr;
		if (needs_conversion)
		{
			raw_local_pose_converted = allocate_type_array_aligned<uint8_t>(allocator, num_bones * transform_size, 64);
			base_local_pose_converted = additive_base_clip != nullptr ? allocate_type_array_aligned<uint8_t>(allocator, num_bones * transform_size, 64) : nullptr;
			lossy_local_pose_converted = allocate_type_array_aligned<uint8_t>(allocator, num_bones * transform_size, 64);
		}

		uint8_t* raw_object_pose = allocate_type_array_aligned<uint8_t>(allocator, num_bones * transform_size, 64);
		uint8_t* lossy_object_pose = allocate_type_array_aligned<uint8_t>(allocator, num_bones * transform_size, 64);

		uint16_t* parent_transform_indices = allocate_type_array<uint16_t>(allocator, num_bones);
		uint16_t* self_transform_indices = allocate_type_array<uint16_t>(allocator, num_bones);

		for (uint16_t transform_index = 0; transform_index < num_bones; ++transform_index)
		{
			const RigidBone& bone = skeleton.get_bone(transform_index);
			parent_transform_indices[transform_index] = bone.parent_index;
			self_transform_indices[transform_index] = transform_index;
		}

		void* raw_local_pose_ = needs_conversion ? (void*)raw_local_pose_converted : (void*)raw_local_pose;
		void* base_local_pose_ = needs_conversion ? (void*)base_local_pose_converted : (void*)base_local_pose;
		void* lossy_local_pose_ = needs_conversion ? (void*)lossy_local_pose_converted : (void*)lossy_remapped_local_pose;

		itransform_error_metric::convert_transforms_args convert_transforms_args_raw;
		convert_transforms_args_raw.dirty_transform_indices = self_transform_indices;
		convert_transforms_args_raw.num_dirty_transforms = num_bones;
		convert_transforms_args_raw.transforms = raw_local_pose;
		convert_transforms_args_raw.num_transforms = num_bones;

		itransform_error_metric::convert_transforms_args convert_transforms_args_base = convert_transforms_args_raw;
		convert_transforms_args_base.transforms = base_local_pose;

		itransform_error_metric::convert_transforms_args convert_transforms_args_lossy = convert_transforms_args_raw;
		convert_transforms_args_lossy.transforms = lossy_remapped_local_pose;

		itransform_error_metric::apply_additive_to_base_args apply_additive_to_base_args_raw;
		apply_additive_to_base_args_raw.dirty_transform_indices = self_transform_indices;
		apply_additive_to_base_args_raw.num_dirty_transforms = num_bones;
		apply_additive_to_base_args_raw.local_transforms = raw_local_pose_;
		apply_additive_to_base_args_raw.base_transforms = base_local_pose_;
		apply_additive_to_base_args_raw.num_transforms = num_bones;

		itransform_error_metric::apply_additive_to_base_args apply_additive_to_base_args_lossy = apply_additive_to_base_args_raw;
		apply_additive_to_base_args_lossy.local_transforms = lossy_local_pose_;

		itransform_error_metric::local_to_object_space_args local_to_object_space_args_raw;
		local_to_object_space_args_raw.dirty_transform_indices = self_transform_indices;
		local_to_object_space_args_raw.num_dirty_transforms = num_bones;
		local_to_object_space_args_raw.parent_transform_indices = parent_transform_indices;
		local_to_object_space_args_raw.local_transforms = raw_local_pose_;
		local_to_object_space_args_raw.num_transforms = num_bones;

		itransform_error_metric::local_to_object_space_args local_to_object_space_args_lossy = local_to_object_space_args_raw;
		local_to_object_space_args_lossy.local_transforms = lossy_local_pose_;

		BoneError bone_error;
		bone_error.error = -1.0F;	// Can never have a negative error, use it so the first sample is used
		DefaultOutputWriter pose_writer(lossy_local_pose, num_output_bones);

		for (uint32_t sample_index = 0; sample_index < num_samples; ++sample_index)
		{
			const float sample_time = rtm::scalar_min(float(sample_index) / sample_rate, clip_duration);

			// We use the nearest sample to accurately measure the loss that happened, if any
			clip.sample_pose(sample_time, sample_rounding_policy::nearest, raw_local_pose, num_bones);

			context.seek(sample_time, sample_rounding_policy::nearest);
			context.decompress_pose(pose_writer);

			// Perform remapping by copying the raw pose first and we overwrite with the decompressed pose if
			// the data is available
			std::memcpy(lossy_remapped_local_pose, raw_local_pose, sizeof(rtm::qvvf) * num_bones);
			for (uint16_t output_index = 0; output_index < num_output_bones; ++output_index)
			{
				const uint16_t bone_index = output_bone_mapping[output_index];
				lossy_remapped_local_pose[bone_index] = lossy_local_pose[output_index];
			}

			if (needs_conversion)
			{
				error_metric.convert_transforms(convert_transforms_args_raw, raw_local_pose_converted);
				error_metric.convert_transforms(convert_transforms_args_lossy, lossy_local_pose_converted);
			}

			if (additive_base_clip != nullptr)
			{
				const float normalized_sample_time = additive_num_samples > 1 ? (sample_time / clip_duration) : 0.0F;
				const float additive_sample_time = additive_num_samples > 1 ? (normalized_sample_time * additive_duration) : 0.0F;
				additive_base_clip->sample_pose(additive_sample_time, sample_rounding_policy::nearest, base_local_pose, num_bones);

				if (needs_conversion)
					error_metric.convert_transforms(convert_transforms_args_base, base_local_pose_converted);

				
				error_metric.apply_additive_to_base(apply_additive_to_base_args_raw, raw_local_pose_);
				error_metric.apply_additive_to_base(apply_additive_to_base_args_lossy, lossy_local_pose_);
			}

			error_metric.local_to_object_space(local_to_object_space_args_raw, raw_object_pose);
			error_metric.local_to_object_space(local_to_object_space_args_lossy, lossy_object_pose);

			for (uint16_t bone_index = 0; bone_index < num_bones; ++bone_index)
			{
				const RigidBone& bone = skeleton.get_bone(bone_index);

				itransform_error_metric::calculate_error_args calculate_error_args;
				calculate_error_args.raw_transform = raw_object_pose + (bone_index * transform_size);
				calculate_error_args.lossy_transform = lossy_object_pose + (bone_index * transform_size);
				calculate_error_args.construct_sphere_shell(bone.vertex_distance);

				const float error = rtm::scalar_cast(error_metric.calculate_error(calculate_error_args));

				if (error > bone_error.error)
				{
					bone_error.error = error;
					bone_error.index = bone_index;
					bone_error.sample_time = sample_time;
				}
			}
		}

		deallocate_type_array(allocator, output_bone_mapping, num_output_bones);
		deallocate_type_array(allocator, raw_local_pose, num_bones);
		deallocate_type_array(allocator, base_local_pose, num_bones);
		deallocate_type_array(allocator, lossy_local_pose, num_output_bones);
		deallocate_type_array(allocator, lossy_remapped_local_pose, num_bones);
		deallocate_type_array(allocator, raw_local_pose_converted, num_bones * transform_size);
		deallocate_type_array(allocator, base_local_pose_converted, num_bones * transform_size);
		deallocate_type_array(allocator, lossy_local_pose_converted, num_bones * transform_size);
		deallocate_type_array(allocator, raw_object_pose, num_bones * transform_size);
		deallocate_type_array(allocator, lossy_object_pose, num_bones * transform_size);
		deallocate_type_array(allocator, parent_transform_indices, num_bones);
		deallocate_type_array(allocator, self_transform_indices, num_bones);

		return bone_error;
	}
}

ACL_IMPL_FILE_PRAGMA_POP
