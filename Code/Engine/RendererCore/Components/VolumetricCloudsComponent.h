#pragma once

#include <Core/World/World.h>
#include <RendererCore/Meshes/MeshComponent.h>

typedef ezComponentManager<class ezVolumetricCloudsComponent, ezBlockStorageType::Compact> ezVolumetricCloudsComponentManager;

class EZ_RENDERERCORE_DLL ezVolumetricCloudsComponent : public ezRenderComponent
{
  EZ_DECLARE_COMPONENT_TYPE(ezVolumetricCloudsComponent, ezRenderComponent, ezVolumetricCloudsComponentManager);

  //////////////////////////////////////////////////////////////////////////
  // ezComponent

public:
  virtual void SerializeComponent(ezWorldWriter& inout_stream) const override;
  virtual void DeserializeComponent(ezWorldReader& inout_stream) override;

protected:
  virtual void Initialize() override;
  virtual void OnActivated() override;


  //////////////////////////////////////////////////////////////////////////
  // ezRenderComponent

public:
  virtual ezResult GetLocalBounds(ezBoundingBoxSphere& ref_bounds, bool& ref_bAlwaysVisible, ezMsgUpdateLocalBounds& ref_msg) override;


  //////////////////////////////////////////////////////////////////////////
  // ezVolumetricCloudsComponent

public:
  ezVolumetricCloudsComponent();
  ~ezVolumetricCloudsComponent();

private:
  void OnMsgExtractRenderData(ezMsgExtractRenderData& msg) const;
  void UpdateMaterials() const;

  ezMeshResourceHandle m_hMesh;
  ezMaterialResourceHandle m_hMaterial;

  ezTexture3DResourceHandle m_hNoiseLut;
};