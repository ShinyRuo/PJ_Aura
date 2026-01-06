--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_InventoryDialogue_C
local M = UnLua.Class()
local Screen = require "Tutorials.Screen"

--function M:Initialize(Initializer)
--end

--function M:PreConstruct(IsDesignTime)
--end

function M:Construct()
    self.InvDiaController= UE.UAuraAbilitySystemLibrary.GetInventoryWidgetController(self)
    self:SetWidgetController(self.InvDiaController)
    self.CloseButton.OnClicked:Add(self,function() self:SetVisibility(UE.ESlateVisibility.Hidden)  end)
    self.InvDiaController:BroadcastInitalValue()
    self.OnVisibilityChanged:Add(self, M.OnVisibilityChangedEvent)
end

function M:Destruct()
    self.CloseButton.OnClicked:Clear()
end


function M:OnVisibilityChangedEvent(InVisibility)
    if InVisibility == UE.ESlateVisibility.Visible or InVisibility == UE.ESlateVisibility.SelfHitTestInvisible then
        self.WBP_InventoryGrids:OnShow()
    elseif InVisibility == UE.ESlateVisibility.Hidden then
        self.WBP_InventoryGrids:OnHidden()
    -- elseif InVisibility == UE.ESlateVisibility.Collapsed then
    --     Screen.Print(self:GetName() .. " is now Collapsed.")

    -- elseif InVisibility == UE.ESlateVisibility.SelfHitTestInvisible then
    --     Screen.Print(self:GetName() .. " is now SelfHitTestInvisible.")
    end
end



--function M:Tick(MyGeometry, InDeltaTime)
--end

return M
