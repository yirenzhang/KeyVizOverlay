#pragma once

#include <cstddef>
#include <cstdint>

namespace keyviz
{
struct KeyBinding
{
    const char* label;
    std::uint32_t keyCode;
    float widthScale;
};

struct KeyRow
{
    const KeyBinding* keys;
    std::size_t keyCount;
};

struct KeyRowSet
{
    const KeyRow* rows = nullptr;
    std::size_t count = 0;
};

struct CustomLayoutEditCommand
{
    bool moveRequested = false;
    int fromRow = -1;
    int fromIndex = -1;
    int toRow = -1;
    int toIndex = -1;

    bool removeRequested = false;
    int removeRow = -1;
    int removeIndex = -1;
};

int GetLayoutPresetCount();
const char* const* GetLayoutPresetLabels();
KeyRowSet GetRowsForPreset(int presetIndex);
void ApplyLayoutPresetDefaults(int presetIndex, float& layoutScale);
bool IsCustomLayoutPreset(int presetIndex);

int GetCustomEditableRowCount();
const char* const* GetCustomEditableRowLabels();
int GetCustomPaletteKeyCount();
const char* const* GetCustomPaletteKeyLabels();
bool AddCustomKeyByPaletteIndex(int paletteIndex, int rowIndex);
bool AddCustomRow();
bool RemoveCustomRow(int rowIndex);
bool GetCustomIncludeMouse();
void SetCustomIncludeMouse(bool includeMouse);
// 兼容旧流程：建议优先使用 presets 文件管理接口。
bool ExportCustomLayout(const char* path);
// 兼容旧流程：建议优先使用 presets 文件管理接口。
bool ImportCustomLayout(const char* path);
int GetCustomPresetFileCount();
const char* const* GetCustomPresetFileLabels();
bool LoadCustomPresetFileByIndex(int index);
bool SaveCustomPresetAs(const char* fileStem);
bool DuplicateCustomPresetFile(int index, const char* targetFileStem);
bool RenameCustomPresetFile(int index, const char* targetFileStem);
bool DeleteCustomPresetFile(int index);
void ApplyCustomLayoutEditCommand(const CustomLayoutEditCommand& command);
void ResetCustomLayoutPreset();
} // namespace keyviz
