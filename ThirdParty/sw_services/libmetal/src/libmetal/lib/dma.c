/*
 * Copyright (c) 2015, Xilinx Inc. and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Xilinx nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <metal/device.h>
#include <metal/log.h>
#include <metal/dma.h>
#include <metal/atomic.h>

int metal_dma_map(struct metal_device *dev,
		  uint32_t dir,
		  struct metal_sg *sg_in,
		  int nents_in,
		  struct metal_sg *sg_out)
{
	int nents_out;

	if (!dev || !sg_in || !sg_out)
		return -EINVAL;
	if (!dev->bus->ops.dev_dma_map)
		return -ENODEV;

	/* memory barrier */
	if (dir == METAL_DMA_DEV_R)
		/* If it is device read, apply memory write fence. */
		atomic_thread_fence(memory_order_release);
	else
		/* If it is device write or device r/w,
		   apply memory r/w fence. */
		atomic_thread_fence(memory_order_acq_rel);
	nents_out = dev->bus->ops.dev_dma_map(dev->bus,
			dev, dir, sg_in, nents_in, sg_out);
	return nents_out;
}

void metal_dma_unmap(struct metal_device *dev,
		  uint32_t dir,
		  struct metal_sg *sg,
		  int nents)
{
	/* memory barrier */
	if (dir == METAL_DMA_DEV_R)
		/* If it is device read, apply memory write fence. */
		atomic_thread_fence(memory_order_release);
	else
		/* If it is device write or device r/w,
		   apply memory r/w fence. */
		atomic_thread_fence(memory_order_acq_rel);

	if (!dev || !dev->bus->ops.dev_dma_unmap || !sg)
		return;
	dev->bus->ops.dev_dma_unmap(dev->bus,
			dev, dir, sg, nents);
}
