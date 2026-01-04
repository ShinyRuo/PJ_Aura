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
end

function M:Construct()
    self:InitializeParams()
    self.InvGridWidgetController = UE.UAuraAbilitySystemLibrary.GetInventoryWidgetController(self)
    self:SetWidgetController(self.InvGridWidgetController)
end

function M:WidgetControllerSet()
    self:InitSlots()
    self.InvGridWidgetController.OnUIInventoryUpdateDelegate:Add(self, M.OnInventoryUpdate)
end

--创建背包格子
function M:InitSlots()
    local Columns = self.InvGridWidgetController.InventoryWidth
    local Rows = self.InvGridWidgetController.InventoryHeight
    local SlotsNum = Columns * Rows
    self.SlotList:ClearChildren() -- 清理panel
    for i=1,Rows do
        for j=1,Columns do
            local SlotWidget = self:CreateSlotWidget()
            self.SlotList:AddChildToGrid(SlotWidget,i,j)
            table.insert(self.inventorySlots, SlotWidget)
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
            local ItemWidget = self:CreateWidget(UE.UUserWidget, self, "WBP_Item_C")
            if ItemWidget then
                -- 从 slotData 中获取物品的左上角坐标
                local ItemRow = slotData.Y
                local ItemColumn = slotData.X

                -- 将物品控件添加到画布上
                local CanvasSlot = self.ItemCanvas:AddChildToCanvas(ItemWidget)
                -- 根据行列坐标设置位置
                CanvasSlot:SetPosition(UE.FVector2D(ItemColumn * 50, ItemRow * 50)) -- 假设每个格子大小为50x50
                CanvasSlot:SetAutoSize(true)

                -- 调用物品控件的函数来设置其显示数据
                ItemWidget:SetItem(slotData.Item)
                table.insert(self.itemWidgets, ItemWidget)
            end
        end
    end
end

--function M:Tick(MyGeometry, InDeltaTime)
--end

function M:OnInventoryUpdate()
    Screen.Print("OnInventoryUpdate")
    self:ClearAllItems()
    self:FillInventory()
end

function M:OnShow()
    Screen.Print(self:GetName() .."OnShow")
    --刷新界面填充背包格子
    self:OnInventoryUpdate()
end

function M:OnHidden()
    Screen.Print(self:GetName() .."OnHidden")
end



return M
