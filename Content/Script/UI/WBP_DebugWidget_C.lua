--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_DebugWidget_C
local M = UnLua.Class()
local Screen = require "Tutorials.Screen"


--function M:Initialize(Initializer)
--end 1215 -363 90

--function M:PreConstruct(IsDesignTime)
--end

function M:Construct()
    self.InvGridWidgetController = UE.UAuraAbilitySystemLibrary.GetInventoryWidgetController(self)
    self:SetWidgetController(self.InvGridWidgetController)

    self:AddDebugButton("Print Aura Position", "DebugTestFun1")
    self:AddDebugButton("Spawn a Item on the floor", "DebugTestFun2")

end

function M:DebugTestFun1()
    Screen.Print("Print Aura Position")
    local PlayerController = UE.UGameplayStatics.GetPlayerController(self, 0)
    if PlayerController and PlayerController.Pawn then
        local Location = PlayerController.Pawn:K2_GetActorLocation()
        Screen.Print(string.format("Lua: Position: X=%.2f, Y=%.2f, Z=%.2f", Location.X, Location.Y, Location.Z))
    end
end
function M:DebugTestFun2()
    Screen.Print("Spawn a Item on the floor")
    local PlayerController = UE.UGameplayStatics.GetPlayerController(self, 0)
    if PlayerController and PlayerController.Pawn then
        local Location = PlayerController.Pawn:K2_GetActorLocation()
        Location.X = Location.X + 10
        UE.UAuraAbilitySystemLibrary.SpawnAItemOnTheFloor(self,Location,"ShortSword",1)
    end
end

--function M:Tick(MyGeometry, InDeltaTime)
--end



-- 初始化调试面板
function M:Initialize(Initializer)
    -- if not Widget then
    --     return
    -- end

    -- -- 添加一个按钮，用于给玩家添加物品
    -- Widget:AddDebugButton("添加一把剑到背包", function()
    --     print("Lua: 正在尝试添加物品...")
    --     local Inventory = GetPlayerInventory()
    --     if Inventory then
    --         -- 假设你有一个名为 "DA_Item_Sword" 的物品数据资产
    --         -- 请替换成你项目中实际的物品资产路径
    --         local ItemObject = UE.UItem.Load("/Game/Aura/DataAssets/Items/DA_Item_Sword.DA_Item_Sword")
    --         if ItemObject then
    --             -- 假设你的InventoryComponent有一个FindEmptySlotAndAddItem函数
    --             local bSuccess = Inventory:FindEmptySlotAndAddItem(ItemObject)
    --             if bSuccess then
    --                 print("Lua: 物品添加成功!")
    --             else
    --                 print("Lua: 物品添加失败，可能是背包满了。")
    --             end
    --         else
    --             print("Lua: 无法加载物品资产！")
    --         end
    --     else
    --         print("Lua: 找不到玩家的库存组件！")
    --     end
    -- end)



end


return M
