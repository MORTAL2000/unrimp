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


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


#ifdef RENDERERRUNTIME_PROFILER


	//[-------------------------------------------------------]
	//[ Includes                                              ]
	//[-------------------------------------------------------]
	#include <inttypes.h>	// For uint32_t, uint64_t etc.


	//[-------------------------------------------------------]
	//[ Namespace                                             ]
	//[-------------------------------------------------------]
	namespace RendererRuntime
	{


		//[-------------------------------------------------------]
		//[ Classes                                               ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Abstract profiler interface
		*/
		class IProfiler
		{


		//[-------------------------------------------------------]
		//[ Public virtual RendererRuntime::IProfiler methods     ]
		//[-------------------------------------------------------]
		public:
			/**
			*  @brief
			*    Begin profiler CPU sample section
			*
			*  @param[in] name
			*    Section name
			*  @param[in] hashCache
			*    Hash cache
			*/
			virtual void beginCpuSample(const char* name, uint32_t* hashCache) = 0;

			/**
			*  @brief
			*    End profiler CPU sample section
			*/
			virtual void endCpuSample() = 0;

			/**
			*  @brief
			*    Begin profiler GPU sample section
			*
			*  @param[in] name
			*    Section name
			*  @param[in] hashCache
			*    Hash cache
			*/
			virtual void beginGpuSample(const char* name, uint32_t* hashCache) = 0;

			/**
			*  @brief
			*    End profiler GPU sample section
			*/
			virtual void endGpuSample() = 0;


		//[-------------------------------------------------------]
		//[ Protected methods                                     ]
		//[-------------------------------------------------------]
		protected:
			inline IProfiler()
			{
				// Nothing here
			}

			inline virtual ~IProfiler()
			{
				// Nothing here
			}

			explicit IProfiler(const IProfiler&) = delete;
			IProfiler& operator=(const IProfiler&) = delete;


		};


	//[-------------------------------------------------------]
	//[ Namespace                                             ]
	//[-------------------------------------------------------]
	} // RendererRuntime


	//[-------------------------------------------------------]
	//[ Macros & definitions                                  ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Begin profiler CPU sample section
	*
	*  @param[in] context
	*    Renderer context to ask for the profiler interface
	*  @param[in] name
	*    Section name
	*/
	#define RENDERER_PROFILER_BEGIN_CPU_SAMPLE(context, name) { static uint32_t sampleHash_##__LINE__ = 0; (context).getProfiler().beginCpuSample(name, &sampleHash_##__LINE__); }

	/**
	*  @brief
	*    End profiler CPU sample section
	*
	*  @param[in] context
	*    Renderer context to ask for the profiler interface
	*/
	#define RENDERER_PROFILER_END_CPU_SAMPLE(context) (context).getProfiler().endCpuSample();

	/**
	*  @brief
	*    Begin profiler GPU sample section
	*
	*  @param[in] context
	*    Renderer context to ask for the profiler interface
	*  @param[in] name
	*    Section name
	*/
	#define RENDERER_PROFILER_BEGIN_GPU_SAMPLE(context, name) { static uint32_t sampleHash_##__LINE__ = 0; (context).getProfiler().beginGpuSample(name, &sampleHash_##__LINE__); }

	/**
	*  @brief
	*    End profiler GPU sample section
	*
	*  @param[in] context
	*    Renderer context to ask for the profiler interface
	*/
	#define RENDERER_PROFILER_END_GPU_SAMPLE(context) (context).getProfiler().endGpuSample();
#else
	//[-------------------------------------------------------]
	//[ Macros & definitions                                  ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Begin profiler CPU sample section
	*
	*  @param[in] context
	*    Renderer context to ask for the profiler interface
	*  @param[in] name
	*    Section name
	*/
	#define RENDERER_PROFILER_BEGIN_CPU_SAMPLE(context, name)

	/**
	*  @brief
	*    End profiler CPU sample section
	*
	*  @param[in] context
	*    Renderer context to ask for the profiler interface
	*/
	#define RENDERER_PROFILER_END_CPU_SAMPLE(context)

	/**
	*  @brief
	*    Begin profiler GPU sample section
	*
	*  @param[in] context
	*    Renderer context to ask for the profiler interface
	*  @param[in] name
	*    Section name
	*/
	#define RENDERER_PROFILER_BEGIN_GPU_SAMPLE(context, name)

	/**
	*  @brief
	*    End profiler GPU sample section
	*
	*  @param[in] context
	*    Renderer context to ask for the profiler interface
	*/
	#define RENDERER_PROFILER_END_GPU_SAMPLE(context)
#endif