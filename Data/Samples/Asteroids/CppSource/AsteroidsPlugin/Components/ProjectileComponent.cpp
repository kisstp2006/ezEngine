#include <AsteroidsPlugin/Components/CollidableComponent.h>
#include <AsteroidsPlugin/Components/ProjectileComponent.h>
#include <AsteroidsPlugin/Components/ShipComponent.h>
#include <AsteroidsPlugin/GameState/Level.h>
#include <Foundation/Configuration/CVar.h>
#include <Foundation/Utilities/Stats.h>
#include <RendererCore/Meshes/MeshComponent.h>

#include <Core/Input/DeviceTypes/Controller.h>
#include <Core/System/ControllerInput.h>

// clang-format off
EZ_BEGIN_COMPONENT_TYPE(ProjectileComponent, 1, ezComponentMode::Dynamic);
EZ_END_COMPONENT_TYPE
// clang-format on

ezCVarFloat CVar_ProjectileTimeToLive("g_ProjectileTimeToLive", 0.5f, ezCVarFlags::Default, "Projectile time to Live");
ezCVarFloat CVar_SparksTimeToLive("g_SparksTimeToLive", 3.0f, ezCVarFlags::Default, "Projectile time to fade out");
ezCVarInt CVar_SparksPerHit("g_SparksPerHit", 50, ezCVarFlags::Default, "Number of particles spawned when projectile hits a ship");
ezCVarFloat CVar_SparksSpeed("g_SparksSpeed", 50.0f, ezCVarFlags::Default, "Projectile fly speed");

ProjectileComponent::ProjectileComponent()
{
  m_TimeToLive = ezTime::MakeFromSeconds(CVar_ProjectileTimeToLive);
}

void ProjectileComponent::Update()
{
  const ezTime tDiff = GetWorld()->GetClock().GetTimeDiff();
  m_TimeToLive -= tDiff;

  if (m_TimeToLive.IsZeroOrNegative())
  {
    GetWorld()->DeleteObjectDelayed(GetOwner()->GetHandle());
    return;
  }

  if (m_fSpeed <= 0.0f)
    return;

  const ezVec3 vVelocity = GetOwner()->GetLocalRotation() * ezVec3(m_fSpeed, 0, 0);
  const ezVec3 vDistance = (float)tDiff.GetSeconds() * vVelocity;
  GetOwner()->SetLocalPosition(GetOwner()->GetLocalPosition() + vDistance);

  CollidableComponentManager* pCollidableManager = GetWorld()->GetOrCreateComponentManager<CollidableComponentManager>();

  for (auto it = pCollidableManager->GetComponents(); it.IsValid(); ++it)
  {
    CollidableComponent& collider = *it;

    if (!collider.IsActiveAndSimulating())
      continue;

    ezGameObject* pColliderObject = collider.GetOwner();
    ShipComponent* pShipComponent = nullptr;

    if (pColliderObject->TryGetComponentOfBaseType(pShipComponent))
    {
      if (pShipComponent->m_iPlayerIndex == m_iBelongsToPlayer)
        continue;

      if (!pShipComponent->IsAlive())
        continue;
    }

    ezBoundingSphere bs = ezBoundingSphere::MakeFromCenterAndRadius(pColliderObject->GetLocalPosition(), collider.m_fCollisionRadius);

    const ezVec3 vPos = GetOwner()->GetLocalPosition();

    if (!vVelocity.IsZero(0.001f) && bs.GetLineSegmentIntersection(vPos, vPos + vDistance))
    {
      if (pShipComponent && m_fDoesDamage > 0.0f)
      {
        pShipComponent->m_fHealth = ezMath::Max(pShipComponent->m_fHealth - m_fDoesDamage, 0.0f);

        {
          float HitTrack[20] = {
            1.0f, 0.1f, 0.0f, 0.1f, 0.0f, 0.1f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

          if (ezControllerInput::HasDevice())
          {
            ezControllerInput::GetDevice()->AddVibrationTrack(static_cast<ezUInt8>(pShipComponent->m_iPlayerIndex), ezInputDeviceController::Motor::LeftMotor, HitTrack, 20);
          }
        }

        const float fAngle = (float)GetWorld()->GetRandomNumberGenerator().DoubleMinMax(10.0, 100.0);
        const float fSteps = fAngle / CVar_SparksPerHit;

        for (ezInt32 i = 0; i < CVar_SparksPerHit; ++i)
        {
          ezQuat qRot = ezQuat::MakeFromAxisAndAngle(ezVec3(0, 0, 1), ezAngle::MakeFromDegree((i - (CVar_SparksPerHit / 2)) * fSteps));

          {
            ezGameObjectDesc desc;
            desc.m_bDynamic = true;
            desc.m_LocalPosition = GetOwner()->GetLocalPosition();
            desc.m_LocalRotation = qRot * GetOwner()->GetLocalRotation();

            ezGameObject* pProjectile = nullptr;
            GetWorld()->CreateObject(desc, pProjectile);

            ProjectileComponent* pProjectileComponent = nullptr;
            ezComponentHandle hProjectileComponent = ProjectileComponent::CreateComponent(pProjectile, pProjectileComponent);

            pProjectileComponent->m_iBelongsToPlayer = pShipComponent->m_iPlayerIndex;
            pProjectileComponent->m_fSpeed = (float)GetWorld()->GetRandomNumberGenerator().DoubleMinMax(1.0, 2.0) * CVar_SparksSpeed;
            pProjectileComponent->m_fDoesDamage = 0.0f;

            // ProjectileMesh
            {
              ezMeshComponent* pMeshComponent = nullptr;
              ezMeshComponent::CreateComponent(pProjectile, pMeshComponent);

              pMeshComponent->SetMesh(ezResourceManager::LoadResource<ezMeshResource>("ProjectileMesh"));

              // this only works because the materials are part of the Asset Collection and get a name like this from there
              // otherwise we would need to have the GUIDs of the 4 different material assets available
              ezStringBuilder sMaterialName;
              sMaterialName.SetFormat("MaterialPlayer{0}", pShipComponent->m_iPlayerIndex + 1);
              pMeshComponent->SetMaterial(0, ezResourceManager::LoadResource<ezMaterialResource>(sMaterialName));
            }
          }
        }
      }

      if (pShipComponent)
      {
        m_TimeToLive = ezTime::MakeFromSeconds(0);
      }
      else
      {
        m_fSpeed = 0.0f;
        m_TimeToLive = ezTime::MakeFromSeconds(CVar_SparksTimeToLive);
      }
    }
  }
}