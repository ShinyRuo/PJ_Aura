# NPC对话系统使用说明

## 系统概述

本NPC对话系统允许玩家通过鼠标悬停和点击与NPC进行对话。系统支持可配置的对话内容和选项，使用数据表格存储对话数据。

## 核心功能

1. **鼠标悬停检测**：当鼠标悬停在NPC上时，鼠标图标变为对话图标
2. **点击触发对话**：在NPC附近点击NPC时触发对话框
3. **可配置对话**：对话内容和选项通过数据表格配置
4. **对话树支持**：支持多层级对话结构
5. **条件对话**：支持基于条件的对话分支
6. **动作执行**：支持对话中的自定义动作

## 系统架构

### 核心组件

| 组件                    | 说明         | 文件位置                                                                                               |
| ----------------------- | ------------ | ------------------------------------------------------------------------------------------------------ |
| NPCInterface            | NPC交互接口  | `Public/Interaction/NPCInterface.h`                                                                  |
| NPCInteractionComponent | NPC交互组件  | `Public/Interaction/NPCInteractionComponent.h` + `Private/Interaction/NPCInteractionComponent.cpp` |
| DialogueData            | 对话数据结构 | `Public/Interaction/DialogueData.h`                                                                  |
| DialogueDataManager     | 对话数据管理 | `Public/Interaction/DialogueDataManager.h` + `Private/Interaction/DialogueDataManager.cpp`         |
| DialogueWidget          | 对话UI组件   | `Public/UI/Widget/Dialogue/DialogueWidget.h` + `Private/UI/Widget/Dialogue/DialogueWidget.cpp`     |

### 集成点

- **AuraPlayerController**：添加了NPC悬停检测和点击检测
- **数据表格**：存储对话内容和选项
- **蓝图**：用于创建NPC实例

## 文件结构

```
PJ_AURA/
├── Public/
│   ├── Interaction/
│   │   ├── NPCInterface.h           # NPC交互接口
│   │   ├── DialogueData.h           # 对话数据结构
│   │   ├── NPCInteractionComponent.h  # NPC交互组件头文件
│   │   └── DialogueDataManager.h      # 对话数据管理器头文件
│   └── UI/
│       └── Widget/
│           └── Dialogue/
│               └── DialogueWidget.h  # 对话UI组件头文件
└── Private/
    ├── Interaction/
    │   ├── NPCInteractionComponent.cpp  # NPC交互组件实现
    │   └── DialogueDataManager.cpp      # 对话数据管理器实现
    └── UI/
        └── Widget/
            └── Dialogue/
                └── DialogueWidget.cpp  # 对话UI组件实现
```

## 使用方法

### 1. 创建对话数据

1. **创建数据表格**：

   - 在Content浏览器中右键点击，选择 `Miscellaneous > Data Table`
   - 选择 `FDialogueTree` 作为行结构
   - 命名为 `DialogueData`
2. **配置对话内容**：

   - 打开数据表格
   - 添加新行，设置 `DialogueID`（如 "MerchantNPC"）
   - 填写 `NPCName`、`NPCPortrait` 等基本信息
   - 设置 `StartNodeID`
   - 添加对话节点，每个节点包含：
     - `NodeID`：唯一节点ID
     - `SpeakerName`：说话者名称
     - `DialogueText`：对话文本
     - `Choices`：对话选项（包含文本和下一个节点ID）
     - `Animation`：可选的动画
     - `VoiceOver`：可选的语音
     - `Condition`：可选的条件
     - `Action`：可选的动作

### 2. 创建NPC

1. **创建NPC蓝图**：

   - 在Content浏览器中右键点击，选择 `Blueprint Class`
   - 选择 `Character` 或 `Actor` 作为父类
   - 命名为 `BP_MerchantNPC`
2. **添加组件**：

   - 在蓝图编辑器中，添加 `NPCInteractionComponent`
   - 设置 `DialogueID` 为数据表格中对应的ID（如 "MerchantNPC"）
   - 设置 `InteractionRange`（默认200.0）
   - 可选：设置 `bShowDebugVisuals` 为true以显示调试可视化
3. **设置碰撞**：

   - 确保NPC的碰撞体设置为可被鼠标检测
   - 碰撞通道设置为 `Visibility`

### 3. 测试对话系统

1. **放置NPC**：

   - 将NPC蓝图拖放到场景中
2. **测试交互**：

   - 运行游戏
   - 移动到NPC附近
   - 观察鼠标悬停时的图标变化
   - 点击NPC开始对话
   - 测试对话选项和流程

## 自定义和扩展

### 扩展对话系统

1. **添加新的对话类型**：

   - 在 `DialogueData.h` 中扩展 `FDialogueNode` 结构
2. **添加对话条件**：

   - 使用 `FDialogueChoice` 和 `FDialogueNode` 中的 `Condition` 字段
   - 在 `DialogueWidget.cpp` 中的 `CheckNodeCondition` 方法中实现条件检查逻辑
3. **添加对话动作**：

   - 使用 `FDialogueChoice` 和 `FDialogueNode` 中的 `Action` 字段
   - 在 `DialogueWidget.cpp` 中的 `ProcessNodeAction` 方法中实现动作执行逻辑

### 条件系统示例

```cpp
// 在DialogueWidget.cpp中的CheckNodeCondition方法中实现
if (Condition.StartsWith("PlayerLevel:"))
{
    FString LevelStr = Condition.Right(Condition.Len() - 12);
    int32 RequiredLevel = FCString::Atoi(*LevelStr);
    // 检查玩家等级
    return PlayerLevel >= RequiredLevel;
}
else if (Condition.StartsWith("HasItem:"))
{
    FString ItemID = Condition.Right(Condition.Len() - 8);
    // 检查玩家是否拥有物品
    return PlayerHasItem(ItemID);
}
else if (Condition.StartsWith("QuestCompleted:"))
{
    FString QuestID = Condition.Right(Condition.Len() - 15);
    // 检查任务是否完成
    return IsQuestCompleted(QuestID);
}
```

### 动作系统示例

```cpp
// 在DialogueWidget.cpp中的ProcessNodeAction方法中实现
if (Action.StartsWith("GiveItem:"))
{
    FString ItemID = Action.Right(Action.Len() - 9);
    // 给予玩家物品
    GiveItemToPlayer(ItemID);
}
else if (Action.StartsWith("StartQuest:"))
{
    FString QuestID = Action.Right(Action.Len() - 10);
    // 开始任务
    StartQuest(QuestID);
}
else if (Action.StartsWith("CompleteQuest:"))
{
    FString QuestID = Action.Right(Action.Len() - 14);
    // 完成任务
    CompleteQuest(QuestID);
}
else if (Action.StartsWith("Teleport:"))
{
    FString LocationName = Action.Right(Action.Len() - 9);
    // 传送玩家
    TeleportPlayer(LocationName);
}
```

## 常见问题

1. **对话不触发**：

   - 检查NPC是否添加了 `NPCInteractionComponent`
   - 检查 `DialogueID` 是否正确设置
   - 检查玩家是否在 `InteractionRange` 范围内
   - 检查数据表格是否正确加载
2. **对话内容不显示**：

   - 检查数据表格是否正确配置
   - 检查 `DialogueID` 是否匹配
   - 检查对话节点是否正确设置
   - 检查 `StartNodeID` 是否有效
3. **鼠标图标不变**：

   - 检查NPC的碰撞设置
   - 检查玩家是否在 `InteractionRange` 范围内
   - 检查碰撞通道是否设置为 `Visibility`
4. **选项按钮不显示**：

   - 检查 `ChoicesVerticalBox` 是否正确绑定
   - 检查对话节点是否有选项
   - 检查选项条件是否满足

## 示例对话流程

1. **玩家接近NPC** → 鼠标悬停 → 鼠标图标变为对话图标
2. **玩家点击NPC** → 弹出对话框 → 显示NPC对话
3. **玩家选择选项** → 显示下一段对话 → 直到对话结束
4. **玩家点击关闭** → 对话框关闭 → 恢复玩家控制

## 性能优化

- **数据加载**：对话数据只在需要时加载
- **内存管理**：对话结束后清理资源
- **UI优化**：使用蓝图UI的性能最佳实践
- **对象池**：考虑使用对象池管理频繁创建的UI元素

## 调试功能

1. **启用调试可视化**：

   - 在NPCInteractionComponent中设置 `bShowDebugVisuals` 为true
   - 这将显示交互范围和状态
2. **启用调试模式**：

   - 在DialogueDataManager中设置 `bDebugMode` 为true
   - 这将输出详细的调试日志

## 未来扩展

- **语音对话**：支持语音文件播放
- **动画同步**：对话时播放NPC动画
- **分支对话**：基于玩家选择的不同对话分支
- **任务系统集成**：对话中接受和完成任务
- **商店系统集成**：对话中打开商店界面
- **多语言支持**：支持多语言对话内容
- **对话历史**：记录和查看对话历史
- **对话跳过**：允许玩家跳过已看过的对话

## 技术细节

### 对话状态机

系统使用状态机管理对话状态：

- `DS_None`：无交互
- `DS_Hovering`：鼠标悬停
- `DS_InConversation`：对话中

### 数据验证

系统在加载对话数据时会验证：

- 对话树结构完整性
- 节点连接有效性
- 节点ID唯一性
- 对话文本非空
- 选项配置正确性

### 错误处理

系统包含完善的错误处理：

- 数据加载失败时的错误回调
- 无效对话结构的错误报告
- 运行时错误的日志记录
- 用户友好的错误消息
