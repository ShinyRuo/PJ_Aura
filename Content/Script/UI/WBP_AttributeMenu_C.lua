--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_AtrributeMenu_C
local M = UnLua.Class()
local Screen = require "Tutorials.Screen"

function M:OnCloseBtnClicked()
    self:RemoveFromParent()
end

function M:OnAttributePointsChanged(PointsNum)
    local addBtnEnable = true
    if PointsNum <= 0 then
        addBtnEnable = false
    end
    self.Row_Strength.AddButton.Button:SetIsEnabled(addBtnEnable)
    self.Row_Intelligence.AddButton.Button:SetIsEnabled(addBtnEnable)
    self.Row_Resilience.AddButton.Button:SetIsEnabled(addBtnEnable)
    self.Row_Vigor.AddButton.Button:SetIsEnabled(addBtnEnable)
end




function M:AssignAttributeAddBtnEvent()
    self.Row_Strength.AddButton.Button.OnClicked:Add(self,function() self.WidgetController:UpgradeAttribute(self.Row_Strength.AttributeTag) end)
    self.Row_Intelligence.AddButton.Button.OnClicked:Add(self,function() self.WidgetController:UpgradeAttribute(self.Row_Intelligence.AttributeTag) end)
    self.Row_Resilience.AddButton.Button.OnClicked:Add(self,function() self.WidgetController:UpgradeAttribute(self.Row_Resilience.AttributeTag) end)
    self.Row_Vigor.AddButton.Button.OnClicked:Add(self,function() self.WidgetController:UpgradeAttribute(self.Row_Vigor.AttributeTag) end)
end


--function M:PreConstruct(IsDesignTime)
--end

function M:Construct()
    self.WidgetController = UE.UAuraAbilitySystemLibrary.GetAttributeMenuWidgetController(self)
    self.CloseButton.Button.OnClicked:Add(self, M.OnCloseBtnClicked)
    self.WidgetController.AttributePointsChangedDelegate:Add(self, M.OnAttributePointsChanged)
    self:AssignAttributeAddBtnEvent()
    self:SetWidgetController(self.WidgetController)

   -- self.WBP_AttributePointsRow:SetWidgetController(self.WidgetController)

    local ArrChildAuraWidgets = self:GetAllChildAuraWidget()

    for i = 1, ArrChildAuraWidgets:Length() do
        ArrChildAuraWidgets:Get(i):SetWidgetController(self.WidgetController)
    end

    self.WidgetController:BroadcastInitalValue()
end

--function M:Tick(MyGeometry, InDeltaTime)
--end

function M:Destruct()
    self.AttributeMenuClosed:Broadcast()
    self.CloseButton.Button.OnClicked:Clear()
    self.WidgetController.AttributePointsChangedDelegate:Clear()
    self.Row_Strength.AddButton.Button.OnClicked:Clear()
    self.Row_Intelligence.AddButton.Button.OnClicked:Clear()
    self.Row_Resilience.AddButton.Button.OnClicked:Clear()
    self.Row_Vigor.AddButton.Button.OnClicked:Clear()
end

return M
