--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_Item_C
local M = UnLua.Class()

function M:Initialize()
end

--function M:PreConstruct(IsDesignTime)
--end

function M:Construct()
    -- 绑定按钮事件
    self.ItemButton.OnClicked:Add(self, self.OnItemClicked)
    self.ItemButton.OnHovered:Add(self, self.OnItemHovered)
    self.ItemButton.OnUnhovered:Add(self, self.OnItemUnhovered)
    -- 初始化UI
    self:UpdateVisual()
end

function M:OnMouseButtonDown(MyGeometry, MouseEvent)
    if MouseEvent:IsMouseButtonDown("RightMouseButton") then
        self:OnItemRightClicked(self.itemData)
        return UE.UWidgetBlueprintLibrary.Handled()
    end
    return UE.UWidgetBlueprintLibrary.Unhandled()
end

function M:UpdateVisual()
    if self.itemData then --todo
        local itemImage = self.itemData:GetItemImage()
        self.ItemImage:SetBrushFromTexture(itemImage, true)
        local stack = self.itemData:GetStack()
        self:UpdateStackAmount(stack)
        -- 处理SocketList等
    end
end

function M:UpdateStackAmount(stack)
    if self.itemData:IsStackable() then --todo
        self.StackText:SetText(tostring(stack))
        self.StackText:SetVisibility(UE.ESlateVisibility.Visible)
    else
        self.StackText:SetVisibility(UE.ESlateVisibility.Hidden)
    end
end

function M:ChangeBackground(InColorAndOpacity)
    self.ItemBackground:SetColorAndOpacity(InColorAndOpacity)
end

function M:Destruct()
    -- 解绑所有事件
    self.ItemButton.OnClicked:RemoveAll(self)
    self.ItemButton.OnHovered:RemoveAll(self)
    self.ItemButton.OnUnhovered:RemoveAll(self)
end

-- 物品被点击时
function M:OnItemClicked()
    -- 选中高亮
    self:ChangeBackground(UE.FLinearColor(0.3, 0.6, 1.0, 1.0))
end

-- 鼠标悬停时
function M:OnItemHovered()
    -- 例如：显示物品Tip、边框高亮
    self:ChangeBackground(UE.FLinearColor(0.8, 0.8, 0.2, 1.0))
    -- 可显示Tooltip
    -- self:ShowTooltip(self.itemData)
end

-- 鼠标移出时
function M:OnItemUnhovered()
    -- 例如：恢复默认背景色，隐藏Tip
    print("Item unhovered")
    self:SetDefaultColor()
    -- self:HideTooltip()
end

--function M:Tick(MyGeometry, InDeltaTime)
--end

return M
