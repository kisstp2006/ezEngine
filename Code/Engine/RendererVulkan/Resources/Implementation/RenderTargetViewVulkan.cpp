#include <RendererVulkanPCH.h>

#include <RendererVulkan/Device/DeviceVulkan.h>
#include <RendererVulkan/Resources/RenderTargetViewVulkan.h>
#include <RendererVulkan/Resources/TextureVulkan.h>

bool IsArrayView(const ezGALTextureCreationDescription& texDesc, const ezGALRenderTargetViewCreationDescription& viewDesc)
{
  return texDesc.m_uiArraySize > 1 || viewDesc.m_uiFirstSlice > 0;
}

ezGALRenderTargetViewVulkan::ezGALRenderTargetViewVulkan(ezGALTexture* pTexture, const ezGALRenderTargetViewCreationDescription& Description)
  : ezGALRenderTargetView(pTexture, Description)
  , m_pRenderTargetView(nullptr)
  , m_pDepthStencilView(nullptr)
  , m_pUnorderedAccessView(nullptr)
{
}

ezGALRenderTargetViewVulkan::~ezGALRenderTargetViewVulkan() {}

ezResult ezGALRenderTargetViewVulkan::InitPlatform(ezGALDevice* pDevice)
{
  // TODO
#if 0
  const ezGALTexture* pTexture = nullptr;
  if (!m_Description.m_hTexture.IsInvalidated())
    pTexture = pDevice->GetTexture(m_Description.m_hTexture);

  if (pTexture == nullptr)
  {
    ezLog::Error("No valid texture handle given for rendertarget view creation!");
    return EZ_FAILURE;
  }

  const ezGALTextureCreationDescription& texDesc = pTexture->GetDescription();
  ezGALResourceFormat::Enum viewFormat = texDesc.m_Format;

  if (m_Description.m_OverrideViewFormat != ezGALResourceFormat::Invalid)
    viewFormat = m_Description.m_OverrideViewFormat;


  ezGALDeviceVulkan* pDXDevice = static_cast<ezGALDeviceVulkan*>(pDevice);

  DXGI_FORMAT DXViewFormat = DXGI_FORMAT_UNKNOWN;

  const bool bIsDepthFormat = ezGALResourceFormat::IsDepthFormat(viewFormat);
  if (bIsDepthFormat)
  {
    DXViewFormat = pDXDevice->GetFormatLookupTable().GetFormatInfo(viewFormat).m_eDepthStencilType;
  }
  else
  {
    DXViewFormat = pDXDevice->GetFormatLookupTable().GetFormatInfo(viewFormat).m_eRenderTarget;
  }

  if (DXViewFormat == DXGI_FORMAT_UNKNOWN)
  {
    ezLog::Error("Couldn't get DXGI format for view!");
    return EZ_FAILURE;
  }

  ID3D11Resource* pDXResource = static_cast<const ezGALTextureVulkan*>(pTexture->GetParentResource())->GetDXTexture();
  const bool bIsArrayView = IsArrayView(texDesc, m_Description);  

  if (bIsDepthFormat)
  {
    D3D11_DEPTH_STENCIL_VIEW_DESC DSViewDesc;
    DSViewDesc.Format = DXViewFormat;

    if (texDesc.m_SampleCount == ezGALMSAASampleCount::None)
    {
      if (!bIsArrayView)
      {
        DSViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        DSViewDesc.Texture2D.MipSlice = m_Description.m_uiMipLevel;
      }
      else
      {
        DSViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
        DSViewDesc.Texture2DArray.MipSlice = m_Description.m_uiMipLevel;
        DSViewDesc.Texture2DArray.FirstArraySlice = m_Description.m_uiFirstSlice;
        DSViewDesc.Texture2DArray.ArraySize = m_Description.m_uiSliceCount;
      }
    }
    else
    {
      if (!bIsArrayView)
      {
        DSViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        // DSViewDesc.Texture2DMS.UnusedField_NothingToDefine;
      }
      else
      {
        DSViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
        DSViewDesc.Texture2DMSArray.FirstArraySlice = m_Description.m_uiFirstSlice;
        DSViewDesc.Texture2DMSArray.ArraySize = m_Description.m_uiSliceCount;
      }
    }

    DSViewDesc.Flags = 0;
    if (m_Description.m_bReadOnly)
      DSViewDesc.Flags |= (D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL);

    if (FAILED(pDXDevice->GetDXDevice()->CreateDepthStencilView(pDXResource, &DSViewDesc, &m_pDepthStencilView)))
    {
      ezLog::Error("Couldn't create depth stencil view!");
      return EZ_FAILURE;
    }
    else
    {
      return EZ_SUCCESS;
    }
  }
  else
  {
    D3D11_RENDER_TARGET_VIEW_DESC RTViewDesc;
    RTViewDesc.Format = DXViewFormat;

    if (texDesc.m_SampleCount == ezGALMSAASampleCount::None)
    {
      if (!bIsArrayView)
      {
        RTViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        RTViewDesc.Texture2D.MipSlice = m_Description.m_uiMipLevel;
      }
      else
      {
        RTViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
        RTViewDesc.Texture2DArray.MipSlice = m_Description.m_uiMipLevel;
        RTViewDesc.Texture2DArray.FirstArraySlice = m_Description.m_uiFirstSlice;
        RTViewDesc.Texture2DArray.ArraySize = m_Description.m_uiSliceCount;
      }
    }
    else
    {
      if (!bIsArrayView)
      {
        RTViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
        // RTViewDesc.Texture2DMS.UnusedField_NothingToDefine;
      }
      else
      {
        RTViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
        RTViewDesc.Texture2DMSArray.FirstArraySlice = m_Description.m_uiFirstSlice;
        RTViewDesc.Texture2DMSArray.ArraySize = m_Description.m_uiSliceCount;
      }
    }

    if (FAILED(pDXDevice->GetDXDevice()->CreateRenderTargetView(pDXResource, &RTViewDesc, &m_pRenderTargetView)))
    {
      ezLog::Error("Couldn't create rendertarget view!");
      return EZ_FAILURE;
    }
    else
    {
      return EZ_SUCCESS;
    }
  }
#endif
  return EZ_SUCCESS;
}

ezResult ezGALRenderTargetViewVulkan::DeInitPlatform(ezGALDevice* pDevice)
{
  // TODO
#if 0
  EZ_GAL_Vulkan_RELEASE(m_pRenderTargetView);
  EZ_GAL_Vulkan_RELEASE(m_pDepthStencilView);
  EZ_GAL_Vulkan_RELEASE(m_pUnorderedAccessView);
#endif
  return EZ_SUCCESS;
}



EZ_STATICLINK_FILE(RendererVulkan, RendererVulkan_Resources_Implementation_RenderTargetViewVulkan);