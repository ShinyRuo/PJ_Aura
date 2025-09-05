--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_SlingshotRock_C
local M = UnLua.Class()
local Screen = require "Tutorials.Screen"
-- function M:Initialize(Initializer)
-- end

-- function M:UserConstructionScript()
-- end
local function DelayResetSphereRedius(self, name)
    UE.UKismetSystemLibrary.Delay(self, 0.3)
    self.Sphere:SetSphereRadius(20,true)
end

function M:ReceiveBeginPlay()
     self.YawRotationRate = UE.UKismetMathLibrary.RandomFloatInRange(-360,360)
     self.RollRotationRate = UE.UKismetMathLibrary.RandomFloatInRange(-360,360)
     self.PitchRotationRate = UE.UKismetMathLibrary.RandomFloatInRange(-360,360)
    -- Screen.Print( self.SlingshotRockMesh:GetName())
    coroutine.resume(coroutine.create(DelayResetSphereRedius), self, "A")
end

-- function M:ReceiveEndPlay()
-- end

function M:ReceiveTick(DeltaSeconds)
   local Yaw = (self.YawRotationRate or 1) * DeltaSeconds
   local Roll = (self.RollRotationRate or 1) * DeltaSeconds
   local Pitch = (self.PitchRotationRate or 1) * DeltaSeconds
   local Rotation = UE.FRotator(Pitch,Yaw,Roll)
   self.SlingshotRockMesh:K2_AddLocalRotation(Rotation,false,nil,false)
end

-- function M:ReceiveAnyDamage(Damage, DamageType, InstigatedBy, DamageCauser)
-- end

-- function M:ReceiveActorBeginOverlap(OtherActor)
-- end

-- function M:ReceiveActorEndOverlap(OtherActor)
-- end

return M
