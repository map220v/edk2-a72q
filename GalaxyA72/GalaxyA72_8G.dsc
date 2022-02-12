[Defines]
  PLATFORM_NAME                  = GalaxyA72_8G
  PLATFORM_GUID                  = 28f1a3bf-193a-47e3-a7b9-5a435eaab2ee
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010019
  OUTPUT_DIRECTORY               = Build/$(PLATFORM_NAME)
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = GalaxyA72/GalaxyA72.fdf

!include GalaxyA72/GalaxyA72.dsc

[BuildOptions.common]
  GCC:*_*_AARCH64_CC_FLAGS = -DMEMORY_8G

[PcdsFixedAtBuild.common]
  # System Memory (8GB)
  gArmTokenSpaceGuid.PcdSystemMemorySize|0x200000000