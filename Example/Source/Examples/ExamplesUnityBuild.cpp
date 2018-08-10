/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


// Amalgamated/unity build

// Recommended example order

// Basics
	#include "Private/Basics/FirstTriangle/FirstTriangle.cpp"
	#include "Private/Basics/FirstIndirectBuffer/FirstIndirectBuffer.cpp"
	#include "Private/Basics/VertexBuffer/VertexBuffer.cpp"
	#include "Private/Basics/FirstTexture/FirstTexture.cpp"
	#include "Private/Basics/FirstRenderToTexture/FirstRenderToTexture.cpp"
	#include "Private/Basics/FirstMultipleRenderTargets/FirstMultipleRenderTargets.cpp"
	#ifndef __ANDROID__
		#include "Private/Basics/FirstMultipleSwapChains/FirstMultipleSwapChains.cpp"
	#endif
	#include "Private/Basics/FirstInstancing/FirstInstancing.cpp"
	#include "Private/Basics/FirstGeometryShader/FirstGeometryShader.cpp"
	#include "Private/Basics/FirstTessellationShader/FirstTessellationShader.cpp"
	#include "Private/Basics/FirstComputeShader/FirstComputeShader.cpp"

// Advanced
	#include "Private/Advanced/FirstGpgpu/FirstGpgpu.cpp"
	#include "Private/Advanced/InstancedCubes/CubeRendererDrawInstanced/BatchDrawInstanced.cpp"
	#include "Private/Advanced/InstancedCubes/CubeRendererDrawInstanced/CubeRendererDrawInstanced.cpp"
	#include "Private/Advanced/InstancedCubes/CubeRendererInstancedArrays/BatchInstancedArrays.cpp"
	#include "Private/Advanced/InstancedCubes/CubeRendererInstancedArrays/CubeRendererInstancedArrays.cpp"
	#include "Private/Advanced/InstancedCubes/InstancedCubes.cpp"
	#include "Private/Advanced/IcosahedronTessellation/IcosahedronTessellation.cpp"

// Runtime
#ifdef RENDERER_RUNTIME
	#ifdef RENDERER_RUNTIME_IMGUI
		#include "Private/Runtime/ImGuiExampleSelector/ImGuiExampleSelector.cpp"
	#endif
	#include "Private/Runtime/FirstMesh/FirstMesh.cpp"
	#include "Private/Runtime/FirstCompositor/FirstCompositor.cpp"
	#include "Private/Runtime/FirstCompositor/CompositorInstancePassFirst.cpp"
	#include "Private/Runtime/FirstCompositor/CompositorPassFactoryFirst.cpp"
	#include "Private/Runtime/FirstScene/FirstScene.cpp"
	#include "Private/Runtime/FirstScene/FreeCameraController.cpp"
	#ifdef RENDERER_RUNTIME_OPENVR
		#include "Private/Runtime/FirstScene/VrController.cpp"
	#endif
#endif
