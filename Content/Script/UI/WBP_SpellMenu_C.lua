--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_SpellMenu_C
local M = UnLua.Class()
local Screen = require "Tutorials.Screen"
local SpellGlobeClassName = "WBP_SpellGlobe_Button_C"
local EquipGlobeClassName = "WBP_EquippedRowButton_C"

local AbilityStatusTags = {
    Abilities_Status_Locked = "Abilities.Status.Locked",
    Abilities_Status_Eligible = "Abilities.Status.Eligible",
    Abilities_Status_Unlocked = "Abilities.Status.Unlocked",
    Abilities_Status_Equipped = "Abilities.Status.Equipped"
}

function M:OnCloseBtnClicked()
    self:RemoveFromParent()
end

--function M:Initialize(Initializer)
--end

--function M:PreConstruct(IsDesignTime)
--end
function M:OnSpellPointsChanged(PointsNum)
    local spellBtnEnable = false
    if PointsNum > 0 and self.curSelectTreeGlobe and self.curSelectTreeGlobe > 0 then
        spellBtnEnable = true
    end
    self.SpellButton.Button:SetIsEnabled(spellBtnEnable)
    self.SpellPoints.TextBlock_Value:SetText(tostring(PointsNum))
    self.curPointsNum = PointsNum
    self:CheckSelectGlobe()
end

function M:HideAllSelect()
    for _, info in pairs(self.Tree_GlobeBtns) do
        info.widget:HideSelect()
    end
end

function M:SelectOne(index)
    self:HideAllSelect()
    if self.WaitEquip then
        self.WBP_EquippedSpellRow:StopAllAnimations()
        if self.WaitEquip == "Offensive" then
            self.WBP_EquippedSpellRow:PlayOffensiveSelection(false)
        elseif self.WaitEquip == "Passive" then
            self.WBP_EquippedSpellRow:PlayPassiveSelection(false)
        end
        self.WaitEquip = nil
    end
    local curSelect = self.curSelectTreeGlobe
    local IndexGlobe = self.Tree_GlobeBtns[index]
    if IndexGlobe then
        if curSelect == index then
            self.curSelectTreeGlobe = -1
            IndexGlobe.widget:PlayDelSelectSound()
        else
            self.curSelectTreeGlobe = index
            IndexGlobe.widget:ShowSelect()
            IndexGlobe.widget:PlaySelectSound()
        end
    end
    self:CheckSelectGlobe()
end

function M:CheckSelectGlobe()
    local curStatusName = "None"

    local curGlobe = self.Tree_GlobeBtns[self.curSelectTreeGlobe]
    if curGlobe then
        curStatusName = curGlobe.StatusTagName
    end

    if curStatusName == AbilityStatusTags.Abilities_Status_Locked then --等级不够 图标上是小锁
        self.SpellButton.Button:SetIsEnabled(false)
        self.EquipButton.Button:SetIsEnabled(false)
    elseif curStatusName == AbilityStatusTags.Abilities_Status_Eligible then -- 等级够了 没有点亮
        if self.curPointsNum > 0 then
            self.SpellButton.Button:SetIsEnabled(true)
        end
        self.EquipButton.Button:SetIsEnabled(false)
    elseif curStatusName == AbilityStatusTags.Abilities_Status_Unlocked then --点亮了 没装备
        if self.curPointsNum > 0 then
            self.SpellButton.Button:SetIsEnabled(true)
        end
        self.EquipButton.Button:SetIsEnabled(true)
    elseif curStatusName == AbilityStatusTags.Abilities_Status_Equipped then --点亮了 没装备
        if self.curPointsNum > 0 then
            self.SpellButton.Button:SetIsEnabled(true)
        end
        self.EquipButton.Button:SetIsEnabled(true)
    else
        self.SpellButton.Button:SetIsEnabled(false)
        self.EquipButton.Button:SetIsEnabled(false)
    end
    self:FillAbilityDesc()
end

function M:EquipOne(index)
    local curSelectTreeGlobe = self.Tree_GlobeBtns[self.curSelectTreeGlobe]
    if not curSelectTreeGlobe then
        return
    end

    local IndexEquipGlobe = self.Equip_GlobeBtns[index]
    if IndexEquipGlobe and self.WidgetController then
        local TargetSlotTagName = UE.UBlueprintGameplayTagLibrary.GetTagName(IndexEquipGlobe.widget.InputTag)
        local TargetIsPassiveGlobe = string.find(TargetSlotTagName, "InputTag.Passive")
        if
            (self.WaitEquip == "Passive" and TargetIsPassiveGlobe) or
                (self.WaitEquip == "Offensive" and not TargetIsPassiveGlobe)
         then
            -- clicked offensive and wait passive will be ignored
            self.WidgetController:SpellRowGlobePressed(
                curSelectTreeGlobe.widget.AbilityTag,
                IndexEquipGlobe.widget.InputTag
            )
            self.curSelectTreeGlobe = -1
        end
    end

end

function M:FindTreeGlobeByTag(TargetTag)
    for index, info in pairs(self.Tree_GlobeBtns) do
        local childTag = info.widget.AbilityTag
        if UE.UBlueprintGameplayTagLibrary.MatchesTag(TargetTag, childTag, true) then
            return index
        end
    end
    return -1
end

function M:FindEquipGlobeBySlotTag(TargetTag)
    for index, info in pairs(self.Equip_GlobeBtns) do
        local childTag = info.widget.InputTag
        if UE.UBlueprintGameplayTagLibrary.MatchesTag(TargetTag, childTag, true) then
            return index
        end
    end
    return -1
end

function M:OnAbilityInfoChanged(AuraAbilityInfo) --升级解锁技能 或者 初始化界面发送所有技能信息
    --if tree globe should change
    local GlobeIndex = self:FindTreeGlobeByTag(AuraAbilityInfo.AbilityTag)
    if GlobeIndex > 0 then
        self.Tree_GlobeBtns[GlobeIndex].StatusTagName =
            UE.UBlueprintGameplayTagLibrary.GetTagName(AuraAbilityInfo.StatusTag)
        self.Tree_GlobeBtns[GlobeIndex].AbilityLevel = AuraAbilityInfo.AbilityLevel
    end
    if GlobeIndex == self.curSelectTreeGlobe then
        self:CheckSelectGlobe()
    end
    --if equip globe should change

    -- GlobeIndex = self:FindEquipGlobeBySlotTag(AuraAbilityInfo.InputTag)
    -- if GlobeIndex > 0 then

    -- end
end

function M:PreConstruct()
    self.SpellButton.Button:SetIsEnabled(false)
    self.EquipButton.Button:SetIsEnabled(false)
end

function M:InitializeParams()
    self.curSelectTreeGlobe = -1
    self.curSelectIsPassive = false
    self.Tree_GlobeBtns = {}
    self.Equip_GlobeBtns = {}
    self.WidgetController = nil
    self.WaitEquip = nil
end

function M:Construct()
    self:InitializeParams()

    self.WidgetController = UE.UAuraAbilitySystemLibrary.GetSpellMenuWidgetController(self)
    self:SetWidgetController(self.WidgetController)
    local ArrChildAuraWidgets = self:GetAllChildAuraWidget()
    -- local widget_class = UE.UClass.Load("/Game/Blueprints/UI/SpellGlobes/WBP_SpellGlobe_Button.WBP_SpellGlobe_Button_C")
    for i = 1, ArrChildAuraWidgets:Length() do
        local child = ArrChildAuraWidgets:Get(i)
        child:SetWidgetController(self.WidgetController)
    end

    self:SetUpTreeGlobes()
    self:SetUpEquipGlobes()

    self.WidgetController.SpellPointsChangedDelegate:Add(self, M.OnSpellPointsChanged)
    self.WidgetController.AbilityInfoDelegate:Add(self, M.OnAbilityInfoChanged)
    self.WidgetController.UIAbilityEquipped:Bind(self, M.OnUIAbilityEquipped)

    --Screen.Print( "UI bind2")

    self.SpellButton.Button.OnClicked:Add(self, M.OnSpellButtonClicked)
    self.EquipButton.Button.OnClicked:Add(self, M.OnEquipButtonClicked)

    self.WidgetController:BroadcastInitalValue()

    self.CloseButton.Button.OnClicked:Add(self, M.OnCloseBtnClicked)
end

function M:SetUpTreeGlobes()
    local SpellTreeChild = self.WBP_OffensiveSpellTree:GetAllChildAuraWidget()
    local SpellTreeIndex = 1
    for i = 1, SpellTreeChild:Length() do
        local child = SpellTreeChild:Get(i)
        local childClass = child:GetClass()
        if childClass:GetName() == SpellGlobeClassName then
            self.Tree_GlobeBtns[SpellTreeIndex] = {}
            self.Tree_GlobeBtns[SpellTreeIndex].widget = child
            self.Tree_GlobeBtns[SpellTreeIndex].StatusTagName = AbilityStatusTags.Abilities_Status_Locked
            self.Tree_GlobeBtns[SpellTreeIndex].AbilityLevel = 0
            --AbilityTag 在蓝图里设置了
            local lamdaCaptureIndex = SpellTreeIndex
            child.Button_Ring.OnClicked:Add(
                self,
                function()
                    self.curSelectIsPassive = false
                    self:SelectOne(lamdaCaptureIndex)
                end
            )
        end
        SpellTreeIndex = SpellTreeIndex + 1
    end

    local PassiveTreeChild = self.WBP_PassiveSpellTree:GetAllChildAuraWidget()
    for i = 1, PassiveTreeChild:Length() do
        local child = PassiveTreeChild:Get(i)
        local childClass = child:GetClass()
        if childClass:GetName() == SpellGlobeClassName then
            self.Tree_GlobeBtns[SpellTreeIndex] = {}
            self.Tree_GlobeBtns[SpellTreeIndex].widget = child
            self.Tree_GlobeBtns[SpellTreeIndex].StatusTagName = AbilityStatusTags.Abilities_Status_Locked
            self.Tree_GlobeBtns[SpellTreeIndex].AbilityLevel = 0
            local lamdaCaptureIndex = SpellTreeIndex
            child.Button_Ring.OnClicked:Add(
                self,
                function()
                    self.curSelectIsPassive = true
                    self:SelectOne(lamdaCaptureIndex)
                end
            )
        end
        SpellTreeIndex = SpellTreeIndex + 1
    end
end

function M:SetUpEquipGlobes()
    local SpellEquipChild = self.WBP_EquippedSpellRow:GetAllChildAuraWidget()
    local SpellEquipIndex = 1

    for i = 1, SpellEquipChild:Length() do
        local child = SpellEquipChild:Get(i)
        local childClass = child:GetClass()
        if childClass:GetName() == EquipGlobeClassName then
            self.Equip_GlobeBtns[SpellEquipIndex] = {}
            self.Equip_GlobeBtns[SpellEquipIndex].widget = child
            --AbilityTag 在蓝图里设置了
            local lamdaCaptureIndex = SpellEquipIndex
            child.Button_Ring.OnClicked:Add(
                self,
                function()
                    self:EquipOne(lamdaCaptureIndex)
                end
            )
        end
        SpellEquipIndex = SpellEquipIndex + 1
    end
end

function M:OnUIAbilityEquipped()
    self.WBP_EquippedSpellRow:StopAllAnimations()
    if self.WaitEquip == "Offensive" then
        self.WBP_EquippedSpellRow:PlayOffensiveSelection(false)
    elseif self.WaitEquip == "Passive" then
        self.WBP_EquippedSpellRow:PlayPassiveSelection(false)
    end
    self.WaitEquip = nil
    self:SelectOne(-1)
end

function M:OnSpellButtonClicked()
    local curSelect = self.curSelectTreeGlobe
    if curSelect and curSelect > 0 and self.Tree_GlobeBtns[curSelect] then
        self.WidgetController:SpendSpellPointPressed(self.Tree_GlobeBtns[curSelect].widget.AbilityTag)
    end
end

function M:OnEquipButtonClicked()
    if self.curSelectTreeGlobe < 1 then
        return
    end

    if self.curSelectIsPassive then
        self.WaitEquip = "Passive"
        self.WBP_EquippedSpellRow:PlayPassiveSelection(true)
    else
        self.WaitEquip = "Offensive"
        self.WBP_EquippedSpellRow:PlayOffensiveSelection(true)
    end

    if self.WaitEquip then
        self.SpellButton.Button:SetIsEnabled(false)
        self.EquipButton.Button:SetIsEnabled(false)
    end
end

function M:FillAbilityDesc()
    self.Description_CurLevel:SetText("")
    self.Description_NextLevel:SetText("")
    local curSelect = self.curSelectTreeGlobe
    if curSelect and curSelect > 0 and self.Tree_GlobeBtns[curSelect] then
        local bSucc, strCurLevelDesc, strNextLevelDesc =
            self.WidgetController:GetAbilityDesc(self.Tree_GlobeBtns[curSelect].widget.AbilityTag)
        self.Description_CurLevel:SetText(strCurLevelDesc)
        self.Description_NextLevel:SetText(strNextLevelDesc)
    end
end

--function M:Tick(MyGeometry, InDeltaTime)
--end

function M:Destruct()
    --WidgetController的delegate是必须清除的 因为 WidgetController是一直存在的
    self.WidgetController.SpellPointsChangedDelegate:Clear()
    self.WidgetController.AbilityInfoDelegate:Clear()

    --下面这些可以不clear 因为每次都重建
    self.SpellMenuClosed:Broadcast()
    self.CloseButton.Button.OnClicked:Clear()
    self.SpellButton.Button.OnClicked:Clear()
end

return M
