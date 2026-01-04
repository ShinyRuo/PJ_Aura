// ALL CODE FOR  learning GAS


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "Net/UnrealNetwork.h"
#include "PJ_AURA/PJ_AURA.h"

AAuraProjectile::AAuraProjectile()
{
	//bReplicates = true 是 Actor 能够进行网络复制的前提，但具体哪些成员变量会被同步，取决于是否使用 Replicated 标记并在 GetLifetimeReplicatedProps 中注册。这种机制让开发者可以精确控制网络同步的数据，避免不必要的网络流量开销。
	bReplicates = true;

	PrimaryActorTick.bCanEverTick = false;
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;

}



void AAuraProjectile::BeginPlay()
{
 	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AAuraProjectile::OnHit()
{
	//在炮弹销毁时又4个情境
	//1 server上炮弹自然消亡 即没达到人
	//2 client上炮弹自然消亡
	//3 server上炮弹打到人了 通过OnSphereOverlap 调用Destroy
	//4 client上炮弹打到人了 由server rep的Destroy
	//那么情况 1和2 时需要播放声音的 但是 1是在server 不需要这个声音 除非声音有什么effect？
	//所以 只有2 需要在这个时候播放声音
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if(LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	bHit = true;
}

void AAuraProjectile::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	if (!bHit && !HasAuthority()) OnHit();//client上 没有打到人 自然消亡
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	if (!bHit) OnHit();

	if (HasAuthority())
	{

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;

			const bool bKnockback = FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance;
			if (bKnockback)
			{
				/*FVector GoingVec = ProjectileMovement->Velocity.GetSafeNormal();
				FRotator Rotation = GoingVec.Rotation();*/
				FRotator Rotation = GetActorRotation();
				Rotation.Pitch = 45.f;
				const FVector KnockbackDirection = Rotation.Vector();
				const FVector KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
				DamageEffectParams.KnockbackForce = KnockbackForce;
			}
			else
			{
				DamageEffectParams.KnockbackForce = FVector::ZeroVector;
			}
			

			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		//打到人了 炮弹需要销毁
		//只由服务器来销毁
		//客户端标记一个bHit
		//不由客户端来销毁的原因我认为是 预测了之后不好callback 这里不能预测？
		Destroy();
	}
}

bool AAuraProjectile::IsValidOverlap(AActor* OtherActor) const
{
	if (!DamageEffectParams.SourceAbilitySystemComponent) 
		return false;
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (SourceAvatarActor == OtherActor) 
		return false;
	if (UAuraAbilitySystemLibrary::IsFriend(SourceAvatarActor, OtherActor)) 
		return false;
	return true;
}




