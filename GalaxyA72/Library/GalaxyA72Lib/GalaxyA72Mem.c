/** @file
*
*  Copyright (c) 2018, Linaro Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>

// The total number of descriptors, including the final "end-of-table" descriptor.
#define MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS 12

// DDR attributes
#define DDR_ATTRIBUTES_CACHED           ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK
#define DDR_ATTRIBUTES_UNCACHED         ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED

#define ATOLL_PERIPH_BASE              0x00000000
#define ATOLL_PERIPH_SZ                0x80000000

STATIC struct GalaxyA72ReservedMemory {
  EFI_PHYSICAL_ADDRESS         Offset;
  EFI_PHYSICAL_ADDRESS         Size;
} GalaxyA72ReservedMemoryBuffer [] = {
  { 0xb0400000, 0x01800000 },    // uh_heap_region
  { 0x80000000, 0x00600000 },    // hyp_region
  { 0x80700000, 0x00140000 },    // xbl_aop_mem
  { 0x808ff000, 0x00001000 },    // sec_apps_region
  { 0x80900000, 0x00200000 },    // smem
  { 0x80b00000, 0x07100000 },    // removed_region
  { 0x8b000000, 0x08900000 },    // modem_region
  { 0x97a00000, 0x00500000 },    // camera_region
  { 0x97f00000, 0x00500000 },    // pil_npu_region
  { 0x98400000, 0x00500000 },    // pil_video_region
  { 0x98900000, 0x01e00000 },    // cdsp_regions
  { 0x93c00000, 0x03e00000 },    // pil_adsp_region
  { 0x93900000, 0x00200000 },    // wlan_fw_region
  { 0x93b00000, 0x00010000 },    // ipa_fw_region
  { 0x93b10000, 0x00005000 },    // ipa_gsi_region
  { 0x93b15000, 0x00002000 },    // gpu_region
  { 0x9e000000, 0x02000000 },    // qseecom_region
  { 0x9f400000, 0x01e00000 },    // cdsp_sec_regions
  { 0x9c000000, 0x01700000 },    // cont_splash_region(framebuffer)
  { 0x9d700000, 0x00100000 },    // dfps_data_region
  { 0x9c000000, 0x01700000 },    // disp_rdump_region(framebuffer)
  { 0xb4400000, 0x00200000 },    // ss_plog
  { 0xb4600000, 0x00100000 },    // ramoops
  { 0xb4700000, 0x00800000 },    // sec_debug_region
  { 0xb4f00000, 0x00001000 },    // sec_debug_autocomment
  //{ 0x0000000180000000, 0x05900000 },    // sec_debug_rdx_bootdev
  { 0xa0001000, 0x00001000 },    // kaslr_region
  { 0xb0200000, 0x00200000 },    // rkp_region
  { 0xa1000000, 0x00001000 },    // hdm_region
  { 0xb5000000, 0x06000000 },    // modem_shared_mem_region
  //{ 0x0000000120000000, 0x25800000 },    // camera_mem_region
};

/**
  Return the Virtual Memory Map of your platform

  This Virtual Memory Map is used by MemoryInitPei Module to initialize the MMU on your platform.

  @param[out]   VirtualMemoryMap    Array of ARM_MEMORY_REGION_DESCRIPTOR describing a Physical-to-
                                    Virtual Memory mapping. This array must be ended by a zero-filled
                                    entry

**/
VOID
ArmPlatformGetVirtualMemoryMap (
  IN ARM_MEMORY_REGION_DESCRIPTOR** VirtualMemoryMap
  )
{
  ARM_MEMORY_REGION_ATTRIBUTES  CacheAttributes;
  ARM_MEMORY_REGION_DESCRIPTOR  *VirtualMemoryTable;
  EFI_RESOURCE_ATTRIBUTE_TYPE   ResourceAttributes;
  UINTN                         Index = 0, Count, ReservedTop;
  EFI_PEI_HOB_POINTERS          NextHob;
  UINT64                        ResourceLength;
  EFI_PHYSICAL_ADDRESS          ResourceTop;

  ResourceAttributes = (
    EFI_RESOURCE_ATTRIBUTE_PRESENT |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_TESTED
  );

  // Create initial Base Hob for system memory.
  BuildResourceDescriptorHob (
    EFI_RESOURCE_SYSTEM_MEMORY,
    ResourceAttributes,
    PcdGet64 (PcdSystemMemoryBase),
    PcdGet64 (PcdSystemMemorySize)
  );

  NextHob.Raw = GetHobList ();
  Count = sizeof (GalaxyA72ReservedMemoryBuffer) / sizeof (struct GalaxyA72ReservedMemory);
  while ((NextHob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, NextHob.Raw)) != NULL)
  {
    if (Index >= Count)
      break;
    if ((NextHob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) &&
        (GalaxyA72ReservedMemoryBuffer[Index].Offset >= NextHob.ResourceDescriptor->PhysicalStart) &&
        ((GalaxyA72ReservedMemoryBuffer[Index].Offset + GalaxyA72ReservedMemoryBuffer[Index].Size) <=
         NextHob.ResourceDescriptor->PhysicalStart + NextHob.ResourceDescriptor->ResourceLength))
    {
      ResourceAttributes = NextHob.ResourceDescriptor->ResourceAttribute;
      ResourceLength = NextHob.ResourceDescriptor->ResourceLength;
      ResourceTop = NextHob.ResourceDescriptor->PhysicalStart + ResourceLength;
      ReservedTop = GalaxyA72ReservedMemoryBuffer[Index].Offset + GalaxyA72ReservedMemoryBuffer[Index].Size;

      // Create the System Memory HOB for the reserved buffer
      BuildResourceDescriptorHob (
        EFI_RESOURCE_MEMORY_RESERVED,
        EFI_RESOURCE_ATTRIBUTE_PRESENT,
        GalaxyA72ReservedMemoryBuffer[Index].Offset,
        GalaxyA72ReservedMemoryBuffer[Index].Size
      );
      // Update the HOB
      NextHob.ResourceDescriptor->ResourceLength = GalaxyA72ReservedMemoryBuffer[Index].Offset -
                                                   NextHob.ResourceDescriptor->PhysicalStart;

      // If there is some memory available on the top of the reserved memory then create a HOB
      if (ReservedTop < ResourceTop)
      {
        BuildResourceDescriptorHob (EFI_RESOURCE_SYSTEM_MEMORY,
                                    ResourceAttributes,
                                    ReservedTop,
                                    ResourceTop - ReservedTop);
      }
      Index++;
    }
    NextHob.Raw = GET_NEXT_HOB (NextHob);
  }

  ASSERT (VirtualMemoryMap != NULL);

  VirtualMemoryTable = (ARM_MEMORY_REGION_DESCRIPTOR*)AllocatePages (
                                                        EFI_SIZE_TO_PAGES (sizeof(ARM_MEMORY_REGION_DESCRIPTOR) * MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS)
                                                        );
  if (VirtualMemoryTable == NULL) {
    return;
  }

  CacheAttributes = DDR_ATTRIBUTES_CACHED;

  Index = 0;

  // DDR - 4.0GB section
  VirtualMemoryTable[Index].PhysicalBase    = PcdGet64 (PcdSystemMemoryBase);
  VirtualMemoryTable[Index].VirtualBase     = PcdGet64 (PcdSystemMemoryBase);
  VirtualMemoryTable[Index].Length          = PcdGet64 (PcdSystemMemorySize);
  VirtualMemoryTable[Index].Attributes      = CacheAttributes;

  // ATOLL SOC peripherals
  VirtualMemoryTable[++Index].PhysicalBase  = ATOLL_PERIPH_BASE;
  VirtualMemoryTable[Index].VirtualBase     = ATOLL_PERIPH_BASE;
  VirtualMemoryTable[Index].Length          = ATOLL_PERIPH_SZ;
  VirtualMemoryTable[Index].Attributes      = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // End of Table
  VirtualMemoryTable[++Index].PhysicalBase  = 0;
  VirtualMemoryTable[Index].VirtualBase     = 0;
  VirtualMemoryTable[Index].Length          = 0;
  VirtualMemoryTable[Index].Attributes      = (ARM_MEMORY_REGION_ATTRIBUTES)0;

  ASSERT((Index + 1) <= MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS);

  *VirtualMemoryMap = VirtualMemoryTable;
}
