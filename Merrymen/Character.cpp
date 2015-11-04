/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/15/2014 3:22:41 PM
------------------------------------------------------------------------------------------
*/

#include "Character.h"
#include "GameScene.h"
#include "PickableBuffCS.h"

// TODO: fix inclusions
#include "../ToneArmEngine/Camera.h"
#include "../ToneArmEngine/RayCasting.h"
#include "../ToneArmEngine/MathHelper.h"
#include "../ToneArmEngine/RunningInfo.h"
#include "../ToneArmEngine/RenderModule.h"
#include "../ToneArmEngine/MeshHelper.h"
#include "../ToneArmEngine/OpenGLRenderModule.h"
#include "../ToneArmEngine/NetworkNode.h"
#include "../ToneArmEngine/SpacePartitionTree.h"
#include "../ToneArmEngine/InputModule.h"
#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/CachedResourceLoader.h"
#include "../ToneArmEngine/InitializationFileResource.h"
#include "../ToneArmEngine/AnimationNode.h"
#include "../ToneArmEngine/ParticleEmitterNodePool.h"
#include "../ToneArmEngine/PhysicsModule.h"
#include "../ToneArmEngine/LevelNode.h"

using namespace merrymen;

IMPLEMENT_RTTI(Character, ClientEntity);

AnimationMap Character::CharacterAnimations = std::map<CharacterAnimationTypes, unsigned long>();

//
// creates a Character object, calls Init() on it
//
Character* Character::Create(const char* modelFile, const char* iniFile, const char* categoryName, bool online) {

	Character* character = new Character();

	if (character && character->Init(modelFile, iniFile, categoryName, online)) {
		return character;
	}

	delete character;

	return nullptr;
}

//
// default contructor
//
Character::Character() :
	m_activeWeapon(nullptr),
	m_stats(nullptr),
	m_statsChanged(false),
	m_aiming(false),
	m_aimingInterrupted(false),
	m_moving(false),
	m_staminaRecharging(false),
	m_sprinting(false),
	m_sprintInterrupted(false),
	m_dead(false),
	m_deathTimer(-1.0f),
	m_sprintInterruptionTimer(-1.0f),
	m_viewHeight(CHARACTER_HEIGHT / 2),
	m_inFOV(false),
	m_teamTag(-1),
	m_kills(0),
	m_deaths(0),
	m_movementDirection(glm::vec3())
{
}

//
// destructor
//
Character::~Character() {

	delete m_stats;
}

//
// initializes a character with the values from a specified .ini file
//
bool Character::Init(const char* modelFile, const char* iniFile, const char* categoryName, bool online) {

	if (ClientEntity::Init(modelFile, online)) {
		
		// initialize character stats
		CachedResourceLoader* loader = Engine::GetInstance()->GetModuleByType<CachedResourceLoader>(EngineModuleType::RESOURCELOADER);
		std::shared_ptr<InitializationFileResource> ini = loader->LoadResource<InitializationFileResource>(iniFile);

		m_stats = new CharacterStats<Character>(this, *ini->GetCategory(categoryName));
		SetStatsChanged(true);

		// initialize animations
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::Idle, StringUtils::Hash("Idle")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkForwardPrimary, StringUtils::Hash("Walk_Forward")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkForwardSecondary, StringUtils::Hash("Walk_Forward")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkForwardAimPrimary, StringUtils::Hash("Walk_Forward_Aim")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkForwardAimSecondary, StringUtils::Hash("Walk_Forward_Aim")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkBackwardPrimary, StringUtils::Hash("Walk_Backward")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkBackwardSecondary, StringUtils::Hash("Walk_Backward")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkBackwardAimPrimary, StringUtils::Hash("Walk_Backward_Aim")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkBackwardAimSecondary, StringUtils::Hash("Walk_Backward_Aim")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkRightPrimary, StringUtils::Hash("Walk_Right")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkRightSecondary, StringUtils::Hash("Walk_Right")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkRightAimPrimary, StringUtils::Hash("Walk_Right_Aim")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkRightAimSecondary, StringUtils::Hash("Walk_Right_Aim")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkLeftPrimary, StringUtils::Hash("Walk_Left")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkLeftSecondary, StringUtils::Hash("Walk_Left")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkLeftAimPrimary, StringUtils::Hash("Walk_Left_Aim")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::WalkLeftAimSecondary, StringUtils::Hash("Walk_Left_Aim")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::AimPrimary, StringUtils::Hash("Primary_Aim")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::AimSecondary, StringUtils::Hash("Secondary_Aim")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::ShootPrimaryHip, StringUtils::Hash("Primary_Shoot_Hip")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::ShootPrimaryAim, StringUtils::Hash("Primary_Shoot_Aim")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::ShootSecondaryHip, StringUtils::Hash("Secondary_Shoot_Hip")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::ShootSecondaryAim, StringUtils::Hash("Secondary_Shoot_Aim")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::ReloadPrimary, StringUtils::Hash("Primary_Load")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::ReloadSecondary, StringUtils::Hash("Secondary_Load")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::CockPrimary, StringUtils::Hash("Primary_Cock")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::CockSecondary, StringUtils::Hash("Secondary_Cock")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::RunPrimary, StringUtils::Hash("Run")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::ChangeWeapon, StringUtils::Hash("Idle")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::RunSecondary, StringUtils::Hash("Run")));
		CharacterAnimations.insert(std::pair<CharacterAnimationTypes, unsigned long>(CharacterAnimationTypes::Die, StringUtils::Hash("Die")));

		m_emitterPool = ParticleEmitterNodePool::CreateEmitterPool( 5 );
		AddChild( m_emitterPool );

		// this will play idle anim
		PlayMovementAnimation();

		return true;
	}

	return false;
}

//
// adds the passed weapon to character's collection of weapons with the specified id
//
void Character::AddWeapon(WeaponID wid, WeaponCS* const weapon) {

	weapon->SetPosition(this->GetComponentOfType<ColliderComponent>()->GetPosition() + glm::vec3(0.0f, 15.0f, 0.0f));
	weapon->SetOwner(this);

	m_weapons[wid] = weapon;
	AddChild(weapon);

	//TODO: Fix this, not sure how.
	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
	SetParentMessage* msg = renderer->GetRenderMessage<SetParentMessage>();
	msg->Init( weapon->GetModel()->GetID(), m_model->GetID() );
	renderer->SendRenderMessage( msg );

	SetParentBoneMessage* boneMsg = renderer->GetRenderMessage<SetParentBoneMessage>();
	boneMsg->Init( weapon->GetModel()->GetID(), StringUtils::Hash( "TankPMC_RightHand" ) );
	renderer->SendRenderMessage( boneMsg );
}

//
// removes the passed weapon from character's collection of weapons
//
void Character::RemoveWeapon(WeaponCS* weapon) {

	WeaponMap::iterator weaponItr = m_weapons.begin();

	while (weaponItr != m_weapons.end()) {

		if (weaponItr->second == weapon) {
			m_weapons.erase(weaponItr);
		}

		weaponItr++;
	}

	RemoveChild(weapon);
}

//
// Character object updates it's state
//
void Character::Update(float dt) {

	if (!IsDead()) {

		// handle single-player stuff
		if (!IsOnline())
		{
			HandleKeyboardInput(dt);	
			HandleMouseInput();
			UpdateStats(dt);
		}

		// handle animations
		CacheAnimationRelatedValues();

		UpdateVisuallEffects(dt);
	}
	else {
		m_deathTimer -= dt;

		// time to respawn?
		if (m_deathTimer < 0.0f && m_deathTimer > -1.0f) {
			Respawn();
		}
	}

	ClientEntity::Update(dt);
}

//
// used is the Single Player mode to update player's health, stamina, etc.
//
void Character::UpdateStats(float dt) {
	
	m_statsChanged = m_stats->Update(dt);

	// player has used all of the character's stamina and has to be punished
	if (m_stats->Stamina < 0.1f) {

		m_sprinting = false;
		m_staminaRecharging = true;
	}
	else if (IsStaminaRecharging() && m_stats->Stamina > m_stats->MaxStamina / 2.0f) {
		m_staminaRecharging = false;
	}

	// player has interrupted their character's sprint with shooting, some time has to pass before the shot will be made
	if (m_sprintInterruptionTimer > 0.0f) {
		m_sprintInterruptionTimer -= dt;
	}

	// let player sprint again
	if (m_sprintInterruptionTimer <= 0.0f && m_sprintInterruptionTimer > -1.0f) {

		m_sprintInterruptionTimer = -1.0f;
		SetSprintInterrupted(false);

		// TODO: find out how this works
		Shoot();
	}
}

//
// handles visuall stuff that needs to be updated regardless of the game mode (SP or MP)
//
void Character::UpdateVisuallEffects(float dt) {

	GameScene* gs = static_cast<GameScene*>(Engine::GetRunningScene());

	if (this == gs->GetLocalCharacter()) {

		// update gui elements
		if (StatsChanged()) {

			SetStatsChanged(false);
			gs->UpdateGUI();
		}

		// update FOV angle
		if (IsAiming() && GetCharacterStats().FOVAngle > GetCharacterStats().DefaultFOVAngle / GetActiveWeapon()->GetWeaponStats().FOVAngleModifier) {
			m_stats->FOVAngle -= dt * FOV_CHANGE_SPEED;
		}
		else if (!IsAiming() && GetCharacterStats().FOVAngle < GetCharacterStats().DefaultFOVAngle) {
			m_stats->FOVAngle += dt * FOV_CHANGE_SPEED;
		}
	}
	// update visibility
	else {

		// fade in
		if (InFOV()) {

			if (GetModel()->GetOpacity() < 1.0f) {

				float opacity = GetModel()->GetOpacity() + dt;
				GetModel()->SetOpacity(opacity);
				GetActiveWeapon()->GetModel()->SetOpacity(opacity);
			}
		}
		// fade out
		else if (GetModel()->IsVisible()) {

			float opacity = GetModel()->GetOpacity() - dt;
			GetModel()->SetOpacity(opacity);
			GetActiveWeapon()->GetModel()->SetOpacity(opacity);
		}

		// hide
		if (GetModel()->IsVisible() && GetModel()->GetOpacity() <= 0.0f) {

			GetModel()->SetVisible(false);
			GetActiveWeapon()->GetModel()->SetVisible(false);
		}
	}

	// update FOV
	if (!IsOnline()) {

		if (this == gs->GetLocalCharacter()) {
			FOVCheck();
		}
	}
	else
	{
		if (NetworkNode::GetLocalPlayer()) {

			Character* localPlayer =  NetworkNode::GetLocalPlayer()->GetParentCharacter();

			// only perform FOV culling for the local player
			if (this == localPlayer) {
				FOVCheck();
			}
		}
	}
}

//
// method that handles keyboard input (SP mode only)
//
void Character::HandleKeyboardInput(float dt) {

	InputModule* mgr = Engine::GetInstance()->GetModuleByType<InputModule>(EngineModuleType::INPUTER);

	if (mgr) {

		// sprinting
		if (mgr->IsKeyPressed(InputModule::ControlElement::Sprint)) {

			// set the timer to default value if the sprint button was released and pressed again
			if (m_sprintInterruptionTimer > -1.0f && !IsSprintInterrupted()) {
				m_sprintInterruptionTimer = -1.0f;
			}

			// sprint if have enough stamina
			if (IsMoving() && m_stats->Stamina > 0.0f && !IsStaminaRecharging() && !IsSprintInterrupted() &&
				glm::dot(GetForward(), m_movementDirection) > (1.0f - MOVEMENT_ANGLE_INTERVAL)) {

				SetSprinting(true);
			}
		}
		else {
			
			SetSprinting(false);

			// player can aim again
			if (IsAimingInterrupted()) {
				SetAimingInterrupted(false);
			}

			// player can sprint again
			if (IsSprintInterrupted()) {
				SetSprintInterrupted(false);
			}
		}

		glm::vec3 movementDirection = glm::vec3();

		// movement
		if (mgr->IsKeyPressed(InputModule::ControlElement::Up)) {
			movementDirection.z -= 1;
		}

		if (mgr->IsKeyPressed(InputModule::ControlElement::Down)) {
			movementDirection.z += 1;
		}

		if (mgr->IsKeyPressed(InputModule::ControlElement::Right)) {
			movementDirection.x += 1;
		}
	
		if (mgr->IsKeyPressed(InputModule::ControlElement::Left)) {
			movementDirection.x -= 1;
		}

		if (movementDirection.x != 0.0f || movementDirection.z != 0.0f) {

			SetMovementDirection(MathHelper::Normalize(movementDirection));
			Move(dt);
		}
		else {
			SetMoving(false);
		}

		// switching between weapons
		if (!IsAiming()) {

			if (mgr->IsKeyPressed(InputModule::ControlElement::PrimaryWeapon)) {
				SetActiveWeapon(GetWeapons().begin()->first);
			}

			if (mgr->IsKeyPressed(InputModule::ControlElement::SecondaryWeapon)) {

				std::unordered_map<WeaponID, WeaponCS*>::iterator weaponItr = GetWeapons().begin();
				weaponItr++;
				SetActiveWeapon(weaponItr->first);
			}
		}

		// reloading
		if (mgr->IsKeyPressed(InputModule::ControlElement::Reload)) {
			Reload();
		}
	}
}

//
// method that handles mouse input (SP mode only)
//
void Character::HandleMouseInput() {
	
	OpenGLRenderModule* rm = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER);
	
	if (rm) {

		// rotation
		glm::mat4x4 camProjMatrix = Engine::GetRunningScene()->GetCurrentCamera()->GetProjectionMatrix();
		glm::mat4x4 camViewMatrix = Engine::GetRunningScene()->GetCurrentCamera()->GetViewMatrix();
		glm::vec4 position = glm::vec4(GetAbsoluteWorldPosition(), 1.0f);

		glm::vec2 screenSize = glm::vec2(Engine::GetInstance()->GetRunningInfo()->contextWidth, Engine::GetInstance()->GetRunningInfo()->contextHeight);

		// project the character onto the screen
		glm::vec2 positionOnScreen = MathHelper::ProjectPointOntoScreen(position);

		// find the position of the mouse inside of the window
		glm::vec2 windowPosition = glm::vec2((float)rm->GetWindow()->getPosition().x, (float)rm->GetWindow()->getPosition().y);

		// get mouse coordinates
		glm::vec2 mousePosition = glm::vec2((float)sf::Mouse::getPosition().x, (float)sf::Mouse::getPosition().y);

		mousePosition -= windowPosition;

		// calculate a vector from the character to the mouse position
		glm::vec2 toMouse = mousePosition - positionOnScreen;

		glm::vec2 intialFwd = glm::vec2(0.0f, 1.0f);

		float angle = MathHelper::AngleBetweenVectors(toMouse, intialFwd);

		if (toMouse.x > 0.0f) {
			SetRotation(glm::vec3(0.0f, angle, 0.0f));
		}
		else if (toMouse.x <= 0.0f) {
			SetRotation(glm::vec3(0.0f, -angle, 0.0f));
		}
	}

	// shooting
	InputModule* iMod = Engine::GetInstance()->GetModuleByType<InputModule>(EngineModuleType::INPUTER);

	if (iMod) {

		if (m_activeWeapon) {

			if (iMod->IsKeyPressed(InputModule::ControlElement::Shoot)) {

				Shoot();
				m_activeWeapon->SetShooting(true);		
			}
			else {
				m_activeWeapon->SetShooting(false);
			}
		}

		// aiming
		if (iMod && GetActiveWeapon()) {
		
			if (iMod->IsKeyPressed(InputModule::ControlElement::Aim)) {

				SetAiming(true);

				if (IsSprinting() && !IsAimingInterrupted()) {

					// stop sprinting
					SetSprinting(false);
					SetSprintInterrupted(true);
				}
			}
			else {
				SetAiming(false);

				// aiming button was released, let player aim again
				if (IsAimingInterrupted()) {
					SetAimingInterrupted(false);
				}

				// let player sprint again, if the sprint was interrupted by aiming
				if (IsSprintInterrupted() && m_sprintInterruptionTimer < 0.0f && !GetActiveWeapon()->IsReloading()) {
					SetSprintInterrupted(false);
				}
			}
		}
	}
}

//
// applies different types of buffs to character's stats
//
void Character::ApplyEffect(const Effect& effect, Character* const author) {

	// TODO: handle all types of buffs
	switch (effect.Type) {

	case EffectType::ExtraAmmo:
		
		m_activeWeapon->GetWeaponStats().ApplyEffect(effect, author);

		break;

	case EffectType::SpeedBuff:

		m_stats->ApplyEffect(effect, author);

	default:
		break;
	}
}

//
// method that translates the character in the game world
//
void Character::Move(float dt) {

	// stop sprinting if player turned left or right
	if (IsSprinting() && (GetMovementDirection().x != 0.0f || GetMovementDirection().z != 0.0f)) {
		SetMovementDirection(GetForward());
	}

	// pick the proper movement speed
	float movementSpeed = m_stats->WalkSpeed;

	if (IsSprinting()) {
		movementSpeed = m_stats->SprintSpeed;
	}
	else if (IsAiming()) {
		movementSpeed = m_stats->WalkSpeedAiming;
	}

	if (GetMovementDirection().x != 0.0f || GetMovementDirection().z != 0.0f) {
		SetPosition(GetPosition() + GetMovementDirection() * movementSpeed * dt);
	}

	SetMoving(true);
}

//
// method that handles shooting commands
//
void Character::Shoot() {

	if (GetActiveWeapon()) {

		if (IsSprinting()) {
			
			// stop sprinting
			SetSprinting(false);
			SetSprintInterrupted(true);

			m_sprintInterruptionTimer = SHOOT_AFTER_SPRINT_DELAY;
		}
		else if (m_sprintInterruptionTimer <= 0.0f) {
			GetActiveWeapon()->HandleShootCommand();
		}
	}
}

//
// method that handles reloading commands
//
void Character::Reload() {

	if (m_activeWeapon && !GetActiveWeapon()->IsReloading()) {

		if (!IsOnline())
		{
			m_activeWeapon->Reload();

			// stop sprinting
			if (IsSprinting()) {

				SetSprinting(false);
				SetSprintInterrupted(true);
			}
		}
		else
		{
			GetActiveWeapon()->SetReloading(true);

			// play sound effect
			GetActiveWeapon()->PlayWeaponSound(WeaponSoundTypes::LoadSound);

			// play animation
			if (GetActiveWeapon()->GetWeaponStats().LoadingUnit == LoadingUnit::Bullet) {
				PlayReloadAnimation(GetActiveWeapon()->GetWeaponStats().Ammo);
			}
			else {
				PlayReloadAnimation(1);
			}
		}
	}
}

//
// method applies damage of different types to the character's stats
//
void Character::TakeDamage(const Damage& damage, ProjectileType projectileType, GameObject* const shooter) {
	
	// deal direct damage to character's stats
	// deal damage to armour
	m_stats->Armour = std::max(0.0f, m_stats->Armour - damage.ArmourDamage());

	// deal damage to hp
	if (m_stats->Armour <= 0 ||
		projectileType == ProjectileType::ArmourPiercingBullet || projectileType == ProjectileType::Shot) {
			
		m_stats->HP = std::max(0.0f, m_stats->HP - damage.HealthDamage());
	}
	
	SetStatsChanged(true);

	// give the user some visual feedback
	PlayTakingDamageEffects();

	// check if the character is still alive
	if (m_stats->HP <= 0.0f) {
		Die(shooter);
	}
}

//
// handles character's death
//
void Character::Die(GameObject* killer) {

	SetDead(true);

	PlayDeathAnimation();

	// drop ammo package
	Effect* ammo = new Effect(EffectType::ExtraAmmo, EffectReason::PickableItem, (m_activeWeapon->GetWeaponStats().MaxAmmo * FRACTION_OF_AMMO_DROPPED));
	PickableBuffCS* ammoPack = nullptr;

	if (IsOnline()) {
		ammoPack = new PickableBuffCS("Models/AmmoBox.vgsModel", ammo, GetAbsoluteWorldPosition(), 50.0f, false);
	}
	else {
		ammoPack = new PickableBuffCS("Models/AmmoBox.vgsModel", ammo, GetAbsoluteWorldPosition(), 50.0f, true);
	}

	GameScene* gs = static_cast<GameScene*>(Engine::GetRunningScene()); 

	if (gs) {
		gs->GetLevel()->AddPickableObject(ammoPack);
	}

	// launch respawn countdown
	if (!IsOnline()) {
		m_deathTimer = RESPAWN_TIMEOUT;
	}
}

//
// respawns the character
//
void Character::Respawn() {

	SetDead(false);

	if (!IsOnline())
	{
		// reset character's stats
		m_stats->HP = m_stats->MaxHP;
		m_stats->Armour = m_stats->MaxArmour;
		m_stats->Stamina = m_stats->MaxStamina;
		const std::vector<glm::vec3>& points = ( (GameScene* )Engine::GetInstance()->GetRunningScene() )->GetLevel()->GetSpawnPoints();
		SetPosition(points[rand() % ( points.size() - 1 )]);

		m_aiming = false;
		m_sprinting	= false;

		// stop respawn countdown
		m_deathTimer = -1.0f;
	}

	m_staminaRecharging = false;

	// reset weapons
	for (auto weapon : m_weapons) {
		weapon.second->Reset();
	}


	SetActiveWeapon(m_weapons.begin()->first);

	SetInFOV(false);

	// this should play idle anim
	PlayMovementAnimation();
}

//
// gives user a visual feedback when their character is being shot
//
void Character::PlayTakingDamageEffects() {

	if (m_model->IsVisible()) {

		// get an emitter
		ParticleEmitterNode* emitter = m_emitterPool->GetEmitterNode();

		// set its parent properly
		Node* parent = emitter->GetParent();

		if (parent && parent != this) {

			emitter->RemoveFromParent();
			emitter->SetPosition(glm::vec3());
		}

		AddChild(emitter);

		// set the position (for now it's gonna be around player's head)
		emitter->SetPosition(glm::vec3(0.0f, 100.0f, 0.0f));

		// blood
		emitter->SetEmitterSettings(ParticleEmitterSettings(500, 0.1f, 0.05f, 5000, 360.0f, glm::vec3(0.5f, 0.0f, 0.0f), 0, 3.0f, 300.0f, 100.0f));
		emitter->Emit(true, 0);
	}
}

//
// makes all players outside of local player's field of view or occluded by some geometry invisible
//
void Character::FOVCheck() {

	// find out which players are inside of the FOV
	GameScene* gs = static_cast<GameScene*>(Engine::GetRunningScene());

	if (gs) {

		std::vector<Character*> playersInFOV;
		
		for (auto otherPlayer : gs->GetPlayers()) {

			if (otherPlayer != this) {

				glm::vec3 toPlayer = otherPlayer->GetAbsoluteWorldPosition() - this->GetAbsoluteWorldPosition();

				glm::vec3 forward = this->GetTransform().GetForward();

				// if the player being checked is outside of the local player's field of view, hide them
				if (MathHelper::AngleBetweenVectors(forward, toPlayer) > m_stats->FOVAngle / 2.0f) {
					otherPlayer->SetInFOV(false);
				}
				else {
					playersInFOV.push_back(otherPlayer);
				}
			}
		}

		Camera* camera = gs->GetCurrentCamera();

		// if there are players inside of local player's FOV, check whether or not they are in the camera frustum
		for (std::vector<Character*>::iterator characterItr = playersInFOV.begin(); characterItr != playersInFOV.end();) {

			if (!gs->GetCurrentCamera()->GetFrustum().BoxInFrustum((*characterItr)->GetComponentOfType<ColliderComponent>()->GetPosition(),
																   (*characterItr)->GetComponentOfType<ColliderComponent>()->GetShape()->GetBoundingDimensions())) {
				(*characterItr)->SetInFOV(false);
				characterItr = playersInFOV.erase(characterItr);
			} else {
				++characterItr;
			}
		}

		// get all the colliders close to the player
		std::vector<TreeNode<ColliderComponent>*> colliderTreeNodes;
		std::vector<ColliderComponent*> colliders;

		BoxShape* boundingBox = BoxShape::CreateBoxShape(glm::vec3(0.0f), glm::vec3(1.0f));
		
		// check whether any of the players which are still visible (playersInFOV) is occluded by any geometry or not
		for (auto player : playersInFOV) {

			ColliderComponent* p_collider = this->GetComponentOfType<ColliderComponent>();

			boundingBox->SetPosition( ( player->GetComponentOfType<ColliderComponent>()->GetPosition() + p_collider->GetPosition() ) * 0.5f );
			boundingBox->SetDimensions( glm::vec3( abs( player->GetComponentOfType<ColliderComponent>()->GetPosition().x - p_collider->GetPosition().x ),
												   p_collider->GetShape()->GetBoundingDimensions().y,
												   abs( player->GetComponentOfType<ColliderComponent>()->GetPosition().z - p_collider->GetPosition().z ) ) );

			PhysicsModule* physics = Engine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);
			physics->GetSpacePartitionTree()->CullBox(*boundingBox, colliderTreeNodes);

			for(std::vector<TreeNode<ColliderComponent>* const>::const_iterator iter = colliderTreeNodes.begin(); iter != colliderTreeNodes.end(); ++iter) {
				( *iter )->GetObjects( colliders );
			}

			// check which of the objects, nearest to the player, are inside of the frustum
			camera->GetFrustum().CollidersInFrustum(colliders);

			glm::vec3 toPlayer = player->GetAbsoluteWorldPosition() - this->GetAbsoluteWorldPosition();
			float distanceToThePlayer = glm::distance(player->GetAbsoluteWorldPosition(), this->GetAbsoluteWorldPosition());
			
			std::shared_ptr<RayCasting::RayCastResult<ColliderComponent>> ptr = RayCasting::RayCastFirst(colliders,
																										 this->GetComponentOfType<ColliderComponent>()->GetPosition(),
																										 MathHelper::Normalize(toPlayer),
																										 distanceToThePlayer);

			if (ptr.get()) {

				RayCasting::RayCastResult<ColliderComponent>* result = ptr.get();

				if (result) {
					
					vgs::ColliderComponent* object = result->Object;
					float distanceToTheObject = glm::distance(this->GetComponentOfType<ColliderComponent>()->GetPosition(), ptr->Intersection);

					// hide the character
					if (object && distanceToTheObject < distanceToThePlayer) {
						player->SetInFOV(false);
					}
				}
			}
			// if the character being checked is inside of the local character's FOV and not occluded by any geometry, make them visible
			else {
				player->SetInFOV(true);
			}

			colliders.clear();
			colliderTreeNodes.clear();
		}

		delete boundingBox;
	}
}

//
// interrupts reloading (if necessary) and sets the boolean variable to the passed value
//
void Character::SetSprinting(const bool sprinting) {

	if (m_sprinting != sprinting) {

		if (sprinting) {

			// stop reloading
			if (m_activeWeapon->IsReloading()) {
				m_activeWeapon->StopReloading();
			}

			// stop aiming
			if (IsAiming()) {

				SetAiming(false);
				SetAimingInterrupted(true);
			}
		}

		m_sprinting = sprinting;
	}
}

//
// changes the FOV of the player and sets the m_aiming value used for the animation system
//
void Character::SetAiming(const bool aiming) {

	if (m_aiming != aiming && (m_online || !GetActiveWeapon()->IsReloading() && (m_online || !IsAimingInterrupted()))) {
		m_aiming = aiming;
	}
}


//
// changes character's position along with the collider's primitive shape's position
//
void Character::SetPosition(const glm::vec3& position) {	

	Node::SetPosition(position);
}

//
// changes the visibility of character's model and their weapon
//
void Character::SetVisible(bool value) {

	if (value != m_model->IsVisible()) {

		m_model->SetVisible(value);
		m_activeWeapon->SetVisible(value);
	}
}

//
// manages visibility based on whether a charatcer is in FOV or not
//
void Character::SetInFOV(bool value) {

	if (value && !GetModel()->IsVisible()) {
		SetVisible(true);
	}

	m_inFOV = value;
}

//
// changes character's avtive weapon
//
void Character::SetActiveWeapon(WeaponID wid) {

	if (m_activeWeapon != m_weapons[wid] && !IsAiming()) {

		// hide the weapon that is no longer being held by the character
		if (m_activeWeapon) {

			m_activeWeapon->SetVisible(false);
			m_activeWeapon->SetActive(false);
		}

		m_activeWeapon = m_weapons[wid];

		if (m_activeWeapon) {

			m_activeWeapon->SetActive(true);

			// show the weapon, that is in the character's hands now
			if (m_model->IsVisible()) {
				m_activeWeapon->SetVisible(true);
			}
		}

		SetStatsChanged(true);

		PlayChangeWeaponAnimation();
	}
}

//
// caches all the values, that the animation system relies on and checks if any of them were changed since the last frame
//
void Character::CacheAnimationRelatedValues() {

	if (!IsDead()) {

		// check if anything needs to be changed

		if (IsMoving()) {
		
			// player started moving
			if (!WasMoving()) {
				PlayMovementAnimation();
			}
			// player changed the direction of their movement
			else if (glm::dot(GetMovementDirection(), GetPreviousMovementDirection()) < (1 - MOVEMENT_ANGLE_INTERVAL) && !IsSprinting()) {
				PlayMovementAnimation();
			}
			// player turned away from where they were facing
			else if (glm::dot(GetPreviousForward(), GetForward()) < (1 - MOVEMENT_ANGLE_INTERVAL) && !IsSprinting()) {
				PlayMovementAnimation();
			}
			// player started sprinting
			else if (IsSprinting() && !WasSprinting()) {
				PlayMovementAnimation();
			}
			// player stopped sprinting
			else if (!IsSprinting() && WasSprinting()) {
				PlayMovementAnimation();
			}
			// player started aiming
			else if (IsAiming() && !WasAiming()) {
				PlayMovementAnimation();
			}
			// player stopped aiming
			else if (!IsAiming() && WasAiming() && !GetActiveWeapon()->IsReloading()) {
				PlayMovementAnimation();
			}
			else if (m_model->GetChildOfType<AnimationNode>()->GetAnimationStatus() == AnimationStatus::STOPPED) {
				PlayMovementAnimation();
			}
		}
		else {
			// player stopped
			if (WasMoving()) {
				PlayMovementAnimation();
			}
			// player started aiming
			else if (IsAiming() && !WasAiming()) {
				PlayMovementAnimation();
			}
			// player stopped aiming
			else if (!IsAiming() && WasAiming() && !GetActiveWeapon()->IsReloading()) {
				PlayMovementAnimation();
			}
			else if (m_model->GetChildOfType<AnimationNode>()->GetAnimationStatus() == AnimationStatus::STOPPED) {
				PlayMovementAnimation();
			}
		}

		SetWasMoving(IsMoving());
		SetWasSprinting(IsSprinting());
		SetWasAiming(IsAiming());
		SetPreviousMovementDirection(GetMovementDirection());
	}
}

//
// figures out which of the shooting animations nee to be played
//
void Character::PlayShootAnimation() {
	if (!IsMoving()) {

		if (GetActiveWeapon() == GetWeapons().begin()->second) {

			if (IsAiming()) {
				m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::ShootPrimaryAim), 1, 0.0f, 1.0f);
			}
			else {
				m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::ShootPrimaryHip), 1, 0.0f, 1.0f);
			}
		}
		else if (GetActiveWeapon() == (++GetWeapons().begin())->second) {

			if (IsAiming()) {
				m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::ShootSecondaryAim), 1, 0.0f, 1.0f);
			}
			else {
				m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::ShootSecondaryHip), 1, 0.0f, 1.0f);
			}
		}
	}
	/*else {
		
		if (GetActiveWeapon() == GetWeapons().begin()->second) {

			if (IsAiming()) {
				m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::ShootPrimaryAim), 0, 0.0f);
			}
			else {
				m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::ShootPrimaryHip), 0, 0.0f);
			}
		}
		else if (GetActiveWeapon() == (GetWeapons().begin()++)->second) {

			if (IsAiming()) {
				m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::ShootSecondaryAim), 0, 0.0f);
			}
			else {
				m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::ShootSecondaryHip), 0, 0.0f);
			}
		}
	}*/
}

//
// figures out which one of the reload animations should be played
//
void Character::PlayReloadAnimation(const int loopCount) {

	if (IsMoving()) {

		//// reload primary weapon while walking
		//if (GetActiveWeapon() == m_weapons.begin()->second) {
		//	m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::ReloadPrimary), loopCount, 0.5f, 1.5f);
		//}
		//// reload secondary weapon while walking
		//else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
		//	m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::ReloadSecondary), loopCount, 0.5f, 1.5f);
		//}
	}
	else {
		// reload primary weapon while standing
		if (GetActiveWeapon() == m_weapons.begin()->second) {
			m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::ReloadPrimary), loopCount, 0.0f, 1.8f);
		}
		// reload secondary weapon while standing
		else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
			m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::ReloadSecondary), loopCount, 0.0f, 0.5f);
		}
	}
}

//
// picks the right cock animation and plays it
//
void Character::PlayCockAnimation() {

	if (!IsMoving()) {

		if (GetActiveWeapon() == m_weapons.begin()->second) {
			m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::CockPrimary), 1, 0.0f, 0.75f);
		}
		// reload secondary weapon while standing
		else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
			m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::CockSecondary), 1, 0.0f, 1.0f); 
		}
	}
}

//
// plays death animation
//
void Character::PlayDeathAnimation() {

	m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::Die), 1, 0.0f, 0.7f);
}

//
// plays change weapon animation
//
void Character::PlayChangeWeaponAnimation() {

	//m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::ChangeWeapon), 1);

	// FIXME: play the actual animation
	PlayMovementAnimation();
}

// figures out which of the movement animations need to be played
//
void Character::PlayMovementAnimation() {

	// play idle animation
	if (!IsMoving() && !IsDead()) {

		if (!IsAiming()) {
			m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::Idle), -1, 0.0f);
		}
		else if (GetActiveWeapon() == m_weapons.begin()->second) {
			m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::AimPrimary), -1, 0.0f);
		}
		else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
			m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::AimSecondary), -1, 0.0f);
		}
	}
	else {

		float dotForward = glm::dot(GetForward(), GetMovementDirection());
		float dotRight = glm::dot(m_transform.GetRight(), GetMovementDirection());

		if (IsSprinting() && dotForward > 1 - MOVEMENT_ANGLE_INTERVAL) {

			if (GetActiveWeapon() == m_weapons.begin()->second) {
				m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::RunPrimary), -1, 0.0f, 1.25f);
			}
			else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
				m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::RunSecondary), -1, 0.0f, 1.25f);
			}
		}
		else {

			if (dotRight > 1 - MOVEMENT_ANGLE_INTERVAL) {

				if (!IsAiming()) {

					if (GetActiveWeapon() == m_weapons.begin()->second) {
						m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkRightPrimary), -1, 0.0f, 1.7f);
					}
					else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
						m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkRightSecondary), -1, 0.0f, 1.7f);
					}
				}
				else if (GetActiveWeapon() == m_weapons.begin()->second) {
					m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkRightAimPrimary), -1, 0.0f, 1.25f);
				}
				else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
					m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkRightAimSecondary), -1, 0.0f, 1.25f);
				}
			}
			else if (dotRight > MOVEMENT_ANGLE_INTERVAL && dotRight < 1 - MOVEMENT_ANGLE_INTERVAL) {

				if (dotForward >= 0) {

					if (IsAiming()) {

						if (GetActiveWeapon() == m_weapons.begin()->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkForwardAimPrimary), 1 - dotRight, -1, 1.25f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkRightAimPrimary), dotRight, -1, 1.25f);
						}
						else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkForwardAimSecondary), 1 - dotRight, -1, 1.25f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkRightAimSecondary), dotRight, -1, 1.25f);
						}
					}
					else {

						if (GetActiveWeapon() == m_weapons.begin()->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkForwardPrimary), 1 - dotRight, -1, 1.7f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkRightPrimary), dotRight, -1, 1.7f);
						}
						else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkForwardSecondary), 1 - dotRight, -1, 1.7f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkForwardSecondary), dotRight, -1, 1.7f);
						}
					}
				}
				else {
					
					if (IsAiming()) {

						if (GetActiveWeapon() == m_weapons.begin()->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkBackwardAimPrimary), 1 - dotRight, -1, 1.25f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkRightAimPrimary), dotRight, -1, 1.25f);
						}
						else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkBackwardAimSecondary), 1 - dotRight, -1, 1.25f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkRightAimSecondary), dotRight, -1, 1.25f);
						}
					}
					else {

						if (GetActiveWeapon() == m_weapons.begin()->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkBackwardPrimary), 1 - dotRight, -1, 1.7f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkRightPrimary), dotRight, -1, 1.7f);
						}
						else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkBackwardSecondary), 1 - dotRight, -1, 1.7f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkBackwardSecondary), dotRight, -1, 1.7f);
						}
					}
				}
			}
			else if (dotRight > -MOVEMENT_ANGLE_INTERVAL) {

				if (dotForward >= 0) {

					if (IsAiming()) {
						
						if (GetActiveWeapon() == m_weapons.begin()->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkForwardAimPrimary), -1, 0.0f, 1.25f);
						}
						else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkForwardAimSecondary), -1, 0.0f, 1.25f);
						}
					}
					else {

						if (GetActiveWeapon() == m_weapons.begin()->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkForwardPrimary), -1, 0.0f, 1.7f);
						}
						else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkForwardSecondary), -1, 0.0f, 1.7f);
						}
					}
				}
				else {
					
					if (IsAiming()) {
						
						if (GetActiveWeapon() == m_weapons.begin()->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkBackwardAimPrimary), -1, 0.0f, 1.25f);
						}
						else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkBackwardAimSecondary), -1, 0.0f, 1.25f);
						}
					}
					else {

						if (GetActiveWeapon() == m_weapons.begin()->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkBackwardPrimary), -1, 0.0f, 1.7f);
						}
						else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkBackwardSecondary), -1, 0.0f, 1.7f);
						}
					}
				}
			}
			else if (dotRight < -MOVEMENT_ANGLE_INTERVAL && dotRight > -1 + MOVEMENT_ANGLE_INTERVAL) {

				if (dotForward >= 0) {

					if (IsAiming()) {

						if (GetActiveWeapon() == m_weapons.begin()->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkForwardAimPrimary), -(-1- dotRight), -1, 1.25f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkLeftAimPrimary), -dotRight, -1, 1.25f);
						}
						else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkForwardAimSecondary), 1 - dotRight, -1, 1.25f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkLeftAimSecondary), dotRight, -1, 1.25f);
						}
					}
					else {

						if (GetActiveWeapon() == m_weapons.begin()->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkForwardPrimary), 1 - dotRight, -1, 1.7f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkLeftPrimary), dotRight, -1, 1.7f);
						}
						else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkForwardSecondary), 1 - dotRight, -1, 1.7f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkLeftSecondary), dotRight, -1, 1.7f);
						}
					}
				}
				else {
					
					if (IsAiming()) {

						if (GetActiveWeapon() == m_weapons.begin()->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkBackwardAimPrimary), 1 - dotRight, -1, 1.25f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkLeftAimPrimary), dotRight, -1, 1.25f);
						}
						else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkBackwardAimSecondary), 1 - dotRight, -1, 1.25f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkLeftAimSecondary), dotRight, -1, 1.25f);
						}
					}
					else {

						if (GetActiveWeapon() == m_weapons.begin()->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkBackwardPrimary), 1 - dotRight, -1, 1.7f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkLeftPrimary), dotRight, -1, 1.7f);
						}
						else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
							m_model->GetChildOfType<AnimationNode>()->PlayBlendedAnimation(0.0f, 2,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkBackwardSecondary), 1 - dotRight, -1, 1.7f,
																						   CharacterAnimations.at(CharacterAnimationTypes::WalkLeftSecondary), dotRight, -1, 1.7f);
						}
					}
				}
			}
			else {

				if (!IsAiming()) {

					if (GetActiveWeapon() == m_weapons.begin()->second) {
						m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkLeftPrimary), -1, 0.0f, 1.7f);
					}
					else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
						m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkLeftSecondary), -1, 0.0f, 1.7f);
					}
				}
				else if (GetActiveWeapon() == m_weapons.begin()->second) {
					m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkLeftAimPrimary), -1, 0.0f, 1.25f);
				}
				else if (GetActiveWeapon() == (++m_weapons.begin())->second) {
					m_model->GetChildOfType<AnimationNode>()->PlayAnimation(CharacterAnimations.at(CharacterAnimationTypes::WalkLeftAimSecondary), -1, 0.0f, 1.25f);
				}
			}
		}

		SetPreviousForward(GetForward());
	}
}