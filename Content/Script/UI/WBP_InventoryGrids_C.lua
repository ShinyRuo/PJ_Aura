--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_InventoryGrids_C
local M = UnLua.Class()
local Screen = require "Tutorials.Screen"

--早于Construct
function M:Initialize(Initializer)
   
end

--function M:PreConstruct(IsDesignTime)
--end

function M:InitializeParams()
    self.InvGridWidgetController  = nil
    self.inventorySlots = {}
    self.itemWidgets = {} -- 用于存储创建的物品控件
    self.bIsDragging = false
end

function M:Construct()
    self:InitializeParams()
    self.InvGridWidgetController = UE.UAuraAbilitySystemLibrary.GetInventoryWidgetController(self)
    self:SetWidgetController(self.InvGridWidgetController)
    self.cachedHoverIndexX = 0
    self.cachedHoverIndexY = 0
    self.DragItemStartX = 0
    self.DragItemStartY = 0
end

function M:WidgetControllerSet()
    self:InitSlots()
    self.InvGridWidgetController.OnUIInventoryUpdateDelegate:Add(self, M.OnInventoryUpdate)
    self:OnInventoryUpdate()
end

--创建背包格子
function M:InitSlots()
    local Columns = self.InvGridWidgetController.InventoryWidth
    local Rows = self.InvGridWidgetController.InventoryHeight
    local SlotsNum = Columns * Rows
    self.SlotList:ClearChildren() -- 清理panel
    for i=1,Rows do
        for j=1,Columns do
            local SlotWidget = self:CreateSlotWidget(i,j)
            self.SlotList:AddChildToGrid(SlotWidget,i,j)
            if not self.inventorySlots[i] then
                self.inventorySlots[i] = {}
            end
            self.inventorySlots[i][j] = SlotWidget
           -- table.insert(self.inventorySlots, SlotWidget)
        end
    end
end

-- 清空所有已显示的物品控件
function M:ClearAllItems()
    for _, itemWidget in ipairs(self.itemWidgets) do
        if itemWidget and itemWidget:IsValid() then
            itemWidget:RemoveFromParent()
        end
    end
    self.itemWidgets = {}
    self.ItemCanvas:ClearChildren()
end


-- 根据背包数据填充物品
function M:FillInventory()
    if not self.InvGridWidgetController or not self.InvGridWidgetController.InventoryComponent then
        return
    end

    local InventoryComponent = self.InvGridWidgetController.InventoryComponent
    local Slots = InventoryComponent.Slots
    local Columns = self.InvGridWidgetController.InventoryWidth
    
    local ProcessedItems = {} -- 用于记录已经处理过的物品，防止重复创建

    -- 使用 pairs 遍历 TArray
    for index, slotData in pairs(Slots) do
        -- 检查格子中是否有物品，并且这个物品我们还没有处理过
        if slotData.Item and slotData.Item:IsValid() and not ProcessedItems[slotData.Item] then
            
            -- 标记这个物品为已处理
            ProcessedItems[slotData.Item] = true

            -- 创建物品控件 (假设你有一个名为WBP_Item的蓝图控件)
            local ItemWidget = self:CreateItemWidget(slotData.Item,slotData.X,slotData.Y)
            if ItemWidget then
                -- 从 slotData 中获取物品的左上角坐标
                local ItemRow = slotData.Y
                local ItemColumn = slotData.X
                local ItemDimensions = slotData.Item:GetItemDimensions()
                ItemWidget.StartX = ItemColumn + 1
                ItemWidget.StartY = ItemRow + 1
                ItemWidget.Dimensions = ItemDimensions
                -- 将物品控件添加到画布上
                local CanvasSlot = self.ItemCanvas:AddChildToCanvas(ItemWidget)
                -- 根据行列坐标设置位置
                CanvasSlot:SetPosition(UE.FVector2D(ItemColumn * 50, ItemRow * 50)) -- 假设每个格子大小为50x50
                CanvasSlot:SetSize(UE.FVector2D(ItemDimensions.X * 50, ItemDimensions.Y * 50))  --根据配表 设置道具的长宽
                -- 调用物品控件的函数来设置其显示数据
                --ItemWidget:SetItem(slotData.Item)
                table.insert(self.itemWidgets, ItemWidget)
            end
        end
    end
end

-- function M:Tick(MyGeometry, InDeltaTime)
    
-- end

function M:OnInventoryUpdate()
    Screen.Print("OnInventoryUpdate")
    self:ClearAllItems()
    self:FillInventory()
    self:ClearSlotColor()
end

function M:OnShow()
    Screen.Print(self:GetName() .."OnShow")
    --刷新界面填充背包格子
    self:OnInventoryUpdate()
end

function M:OnHidden()
    Screen.Print(self:GetName() .."OnHidden")
end

function M:ClearSlotColor()
    if not self.SlotCollect then
        self.SlotCollect = {}
    end
    for index, SlotWidget in ipairs(self.SlotCollect) do
        if SlotWidget and SlotWidget:IsValid() then
            SlotWidget:RemoveColor()
        end
    end
    self.SlotCollect = {}

end

function M:OnSlotDragOver(RowIndex,ColunmIndex,ItemData)
    if RowIndex ==  self.cachedHoverIndexX and  ColunmIndex == self.cachedHoverIndexY then
        return
    end
    self.cachedHoverIndexX = ColunmIndex
    self.cachedHoverIndexY = RowIndex
    local ItemDimensionX = ItemData:GetItemDimensions().X
    local ItemDimensionY = ItemData:GetItemDimensions().Y
    local ItemNeedSpace = ItemDimensionX* ItemDimensionY
    local StartSlot = self.inventorySlots[RowIndex][ColunmIndex]
    self.DropCheckPass = true

    self:ClearSlotColor()

    local NeedMap = {}

    if StartSlot then
        --背包边界检测
        for i= 1,ItemDimensionY do
            for j =1,ItemDimensionX do
                local CheckRowIndex = RowIndex+i-1
                local CheckColunmIndex = ColunmIndex+j-1
                if not self.inventorySlots[CheckRowIndex] then
                    self.DropCheckPass = false
                else
                    local CheckSlot = self.inventorySlots[CheckRowIndex][CheckColunmIndex]
                    if CheckSlot then
                        table.insert(self.SlotCollect, CheckSlot)
                        if not NeedMap[CheckRowIndex] then
                            NeedMap[CheckRowIndex] = {}
                        end
                        NeedMap[CheckRowIndex][CheckColunmIndex] = true
                    else
                        self.DropCheckPass = false
                    end
                end
            end
        end
        --是否覆盖到别的item上
        for index, ItemWidget in ipairs(self.itemWidgets) do
            if ItemWidget.itemData ~= ItemData then
                --拖起来之后 又放到自己的位置上 则不管
                 for i= 1,ItemWidget.Dimensions.Y do
                    for j =1,ItemWidget.Dimensions.X do
                        local CheckX = ItemWidget.StartX + i - 1
                        local CheckY = ItemWidget.StartY + j - 1
                        if NeedMap[CheckX] and NeedMap[CheckX][CheckY] then
                            self.DropCheckPass = false
                        end
                    end
                end
            else
                --源item 记录下位子
                self.DragItemStartX = ItemWidget.StartX
                self.DragItemStartY = ItemWidget.StartY
            end
        end
        for index, SlotWidget in ipairs(self.SlotCollect) do
           if self.DropCheckPass then
                SlotWidget:ShowGreenColor()
           else
                SlotWidget:ShowRedColor()
           end
        end
    end
end

function M:OnItemDropOnSlot()
    if not self.DropCheckPass then
        return false
    end

    self:ClearSlotColor()
    Screen.Print("OnItemDropOnSlot"..self.DragItemStartX..","..self.DragItemStartY..","..self.cachedHoverIndexX..","..self.cachedHoverIndexY)

    self.InvGridWidgetController:TryMoveBagItem( self.DragItemStartX,self.DragItemStartY,self.cachedHoverIndexX,self.cachedHoverIndexY)

    return true
end

function M:OnItemDropOut(ItemStartX,ItemStartY)
   
    self.InvGridWidgetController:TryDropItem( ItemStartX,ItemStartY)

end

function M:OnItemDragCancel()
    self:ClearSlotColor()
end

return M
