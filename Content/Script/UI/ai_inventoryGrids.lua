-- ---@class WBP_InventoryGrid_C : UUserWidget
-- ---@field public ItemCanvas UCanvasPanel
-- ---@field public BackgroundScroll UScrollBox
-- ---@field public ItemScroll UScrollBox
-- ---@field public SlotList UGridPanel
-- ---@field public DragDropButton UButton
-- ---@field private InventoryComponent UInventoryComponent -- 假设类型为 UInventoryComponent
-- ---@field private TileSize number
-- ---@field private Columns number
-- ---@field private Rows number
-- ---@field private LineColor FLinearColor
-- ---@field private HighlightColor FLinearColor
-- ---@field private CanDropColor FLinearColor
-- ---@field private CannotDropColor FLinearColor
-- ---@field private ItemSlots TArray<UUserWidget> -- 假设格子是 WBP_InventorySlot
-- ---@field private LineSegments TArray<UUserWidget> -- 假设是用于画线的Widget
-- ---@field private DraggedItem UItemData -- 假设类型为 UItemData
-- ---@field private DraggedItemCoordinate FIntPoint
-- ---@field private bIsDragging boolean
-- local WBP_InventoryGrid = UnLua.Class()

-- -- 构造函数：绑定UI事件和初始化变量
-- function WBP_InventoryGrid:Construct()
--     self.bIsDragging = false
--     self.ItemSlots = TArray(UUserWidget)
--     self.LineSegments = TArray(UUserWidget)

--     -- 绑定拖拽按钮的按下事件，用于启动拖拽检测
--     self.DragDropButton.OnPressed:Add(self, "HandleDragDropButtonPressed")
-- end

-- -- 析构函数：解绑所有委托，防止内存泄漏
-- function WBP_InventoryGrid:Destruct()
--     if self.InventoryComponent then
--         self.InventoryComponent.OnInventoryUpdated:Remove(self, "RefreshInventoryItems")
--         self.InventoryComponent.OnItemAdded:Remove(self, "OnItemAdded")
--         self.InventoryComponent.OnItemRemoved:Remove(self, "OnItemRemoved")
--     end
-- end

-- -- 核心初始化函数，由外部调用
-- ---@param InInventoryComponent UInventoryComponent
-- function WBP_InventoryGrid:Initialize(InInventoryComponent)
--     if not InInventoryComponent then
--         return
--     end

--     self.InventoryComponent = InInventoryComponent
--     self.Columns = self.InventoryComponent:GetColumns()
--     self.Rows = self.InventoryComponent:GetRows()
--     self.TileSize = 50 -- 假设格子大小为50

--     -- 绑定背包组件的委托
--     self.InventoryComponent.OnInventoryUpdated:Add(self, "RefreshInventoryItems")
--     self.InventoryComponent.OnItemAdded:Add(self, "OnItemAdded")
--     self.InventoryComponent.OnItemRemoved:Add(self, "OnItemRemoved")

--     -- 创建UI
--     self:CreateSlots()
--     self:RefreshInventoryItems()
-- end

-- -- 创建背景格子
-- function WBP_InventoryGrid:CreateSlots()
--     self.SlotList:ClearChildren()
--     for y = 0, self.Rows - 1 do
--         for x = 0, self.Columns - 1 do
--             -- 假设 WBP_InventorySlot_C 是格子的蓝图类
--             local SlotWidget = CreateWidget(self, "WBP_InventorySlot_C")
--             if SlotWidget then
--                 SlotWidget:SetTileSize(self.TileSize)
--                 self.SlotList:AddChildToGrid(SlotWidget, y, x)
--                 self.ItemSlots:Add(SlotWidget)
--             end
--         end
--     end
-- end

-- -- 刷新整个背包的物品显示
-- function WBP_InventoryGrid:RefreshInventoryItems()
--     self.ItemCanvas:ClearChildren()
--     if not self.InventoryComponent then
--         return
--     end

--     local AllItems = self.InventoryComponent:GetItems() -- 假设有此方法
--     for Item in AllItems do
--         self:OnItemAdded(Item, {}) -- 第二个参数是空的 allIndexes
--     end
-- end

-- -- 当一个物品被添加到背包时，创建对应的UI
-- ---@param Item UItemData
-- ---@param AllIndexes TArray<int32>
-- function WBP_InventoryGrid:OnItemAdded(Item, AllIndexes)
--     -- 假设 WBP_InventoryItem_C 是物品的蓝图类
--     local ItemWidget = CreateWidget(self, "WBP_InventoryItem_C")
--     if ItemWidget then
--         ItemWidget:Initialize(Item, self.InventoryComponent) -- 初始化物品UI
        
--         local CanvasSlot = self.ItemCanvas:AddChildToCanvas(ItemWidget)
--         local ItemCoordinate = Item:GetCoordinate() -- 假设物品数据中有坐标
--         CanvasSlot:SetPosition(FVector2D(ItemCoordinate.X * self.TileSize, ItemCoordinate.Y * self.TileSize))
        
--         -- 绑定物品UI的事件
--         ItemWidget.OnItemRightClicked:Add(self, "HandleItemRightClicked")
--     end
-- end

-- -- 当一个物品被移除时
-- ---@param Item UItemData
-- ---@param AllIndexes TArray<int32>
-- function WBP_InventoryGrid:OnItemRemoved(Item, AllIndexes)
--     -- 简单粗暴地刷新整个UI
--     self:RefreshInventoryItems()
-- end

-- -- 处理拖拽按钮按下，准备开始拖拽
-- function WBP_InventoryGrid:HandleDragDropButtonPressed()
--     -- 在OnMouseMove中，我们会找到鼠标下的物品，然后在这里启动拖拽
--     -- 蓝图中使用 DetectDragIfPressed，这里简化为在MouseMove中处理
-- end

-- -- UMG内置事件：鼠标移动
-- ---@param InGeometry FGeometry
-- ---@param InMouseEvent FPointerEvent
-- ---@return FEventReply
-- function WBP_InventoryGrid:OnMouseMove(InGeometry, InMouseEvent)
--     Super.OnMouseMove(self, InGeometry, InMouseEvent)

--     local LocalMousePosition = InGeometry:AbsoluteToLocal(InMouseEvent:GetScreenSpacePosition())
--     local Coordinate = self:MousePositionToCoordinate(LocalMousePosition)

--     if self.bIsDragging and self.DraggedItem then
--         -- 正在拖拽，更新高亮
--         local bCanDrop = self:IsRoomAvailableForDrop(self.DraggedItem, Coordinate)
--         self:SetHighlight(Coordinate, bCanDrop)
--     end

--     -- 如果按下了左键，检测是否可以开始拖拽
--     if InMouseEvent:IsMouseButtonDown(EKeys.LeftMouseButton) and not self.bIsDragging then
--         local ItemUnderCursor = self.InventoryComponent:GetItemAtCoordinate(Coordinate)
--         if ItemUnderCursor then
--             -- 模拟蓝图中的 DetectDrag
--             local DragOp = UWidgetBlueprintLibrary.CreateDragDropOperation(UDragDropOperation) -- 使用你的拖拽操作类
--             DragOp.Payload = ItemUnderCursor -- 传递物品
--             DragOp.DefaultDragVisual = self -- 或者一个专门的拖拽视觉UI
            
--             self.DraggedItem = ItemUnderCursor
--             self.DraggedItemCoordinate = Coordinate
--             self.bIsDragging = true

--             UWidgetBlueprintLibrary.DetectDragAndDrop(self, DragOp, self.DragDropButton, EKeys.LeftMouseButton)
--         end
--     end

--     return UWidgetBlueprintLibrary.Handled()
-- end

-- -- UMG内置事件：处理拖放操作
-- ---@param InGeometry FGeometry
-- ---@param InDragDropEvent FPointerEvent
-- ---@param InOperation UDragDropOperation
-- ---@return boolean
-- function WBP_InventoryGrid:OnDrop(InGeometry, InDragDropEvent, InOperation)
--     Super.OnDrop(self, InGeometry, InDragDropEvent, InOperation)
    
--     self.bIsDragging = false
--     self:ClearHighlight()

--     local DroppedItem = InOperation.Payload
--     if not DroppedItem then
--         return false
--     end

--     local LocalMousePosition = InGeometry:AbsoluteToLocal(InDragDropEvent:GetScreenSpacePosition())
--     local TargetCoordinate = self:MousePositionToCoordinate(LocalMousePosition)

--     -- 尝试将物品移动到新位置
--     local bSuccess = self.InventoryComponent:TryMoveItem(DroppedItem, TargetCoordinate)
--     if not bSuccess then
--         -- 如果移动失败（例如，目标位置有物品），可以尝试交换
--         -- 此处省略交换逻辑以简化
--         print("Failed to move item")
--     end

--     self.DraggedItem = nil
--     return true
-- end

-- -- 将鼠标在Widget内的局部位置转换为网格坐标
-- ---@param MousePosition FVector2D
-- ---@return FIntPoint
-- function WBP_InventoryGrid:MousePositionToCoordinate(MousePosition)
--     local x = math.floor(MousePosition.X / self.TileSize)
--     local y = math.floor(MousePosition.Y / self.TileSize)
--     return FIntPoint(x, y)
-- end

-- -- 检查指定坐标是否能容纳拖拽的物品
-- ---@param ItemToDrop UItemData
-- ---@param TopLeftCoordinate FIntPoint
-- ---@return boolean
-- function WBP_InventoryGrid:IsRoomAvailableForDrop(ItemToDrop, TopLeftCoordinate)
--     if not self.InventoryComponent then
--         return false
--     end
--     -- 假设InventoryComponent有此方法
--     return self.InventoryComponent:IsRoomAvailable(ItemToDrop, TopLeftCoordinate)
-- end

-- -- 设置高亮效果
-- ---@param TopLeftCoordinate FIntPoint
-- ---@param bCanDrop boolean
-- function WBP_InventoryGrid:SetHighlight(TopLeftCoordinate, bCanDrop)
--     self:ClearHighlight()
--     if not self.DraggedItem then return end

--     local Color = bCanDrop and self.CanDropColor or self.CannotDropColor
    
--     -- 假设InventoryComponent有此方法
--     local AllIndexes = self.InventoryComponent:GetAllIndexesFromCoordinate(TopLeftCoordinate, self.DraggedItem:GetDimensions())
    
--     for index in AllIndexes do
--         if self.ItemSlots:IsValidIndex(index) then
--             local SlotWidget = self.ItemSlots:Get(index)
--             SlotWidget:SetHighlight(Color) -- 假设WBP_InventorySlot有此方法
--         end
--     end
-- end

-- -- 清除所有高亮
-- function WBP_InventoryGrid:ClearHighlight()
--     for SlotWidget in self.ItemSlots do
--         SlotWidget:SetHighlight(FLinearColor(0,0,0,0)) -- 透明
--     end
-- end

-- -- 处理物品右键点击事件
-- ---@param ItemWidget UWBP_InventoryItem_C
-- function WBP_InventoryGrid:HandleItemRightClicked(ItemWidget)
--     if self.InventoryComponent and ItemWidget.ItemData then
--         -- 调用背包组件的函数来处理右键逻辑（例如使用物品）
--         self.InventoryComponent:UseItem(ItemWidget.ItemData)
--     end
-- end

-- return WBP_InventoryGrid