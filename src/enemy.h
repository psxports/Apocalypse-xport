#ifndef ENEMY_H
#define ENEMY_H

#include "vector3.h"
#include "character.h"
#include "record_data.h"

uint32 ScaleDamageForDifficulty(uint32 damage);
void ResetEnemyPopulation();

class Enemy : public Character
{
public:
    struct Control
    {
        uint8 script_active;
        uint8 hierarchy_ready;
        uint8 gravity_enabled;
        uint8 damage_phase;
        uint8 activation_count;
        uint8 linked_records_activated;
        uint8 ground_period;
        uint8 ground_tick;
        uint16 prev_activation_flags;
        uint16 variant;
        uint16 height_limit;
        uint32 flags;
        uint16 script_delay;

        Control() : script_active(0), hierarchy_ready(0), gravity_enabled(0), damage_phase(0), activation_count(0), linked_records_activated(0), ground_period(0), ground_tick(0), prev_activation_flags(0), variant(0), height_limit(32), flags(0), script_delay(0)
        {
        }
    } control;

    virtual ~Enemy();
    RecordCursor script_cursor;
    uint32 ReadScriptValue();
    uint32 SkipConditionalBlock();
    uint32 ExecuteScript(const RecordCursor &script);
    uint32 ReadParameters(uint32 command);
    uint32 IntegrateMotion();
    uint32 ChoosePosition(uint32 *output, const uint32 *target, uint32 minimum, uint32 maximum, uint32 height_limit, uint32 spread, uint32 ground, uint32 check_collision);

    struct AI
    {
        // Bytecode parameters and work slots have class-specific meanings
        uint16 parameters[10];
        uint16 work[6];
        uint16 state;
        uint16 phase;
        // Slot nine of the bytecode register bank has no named behavior consumer
        uint16 script_register9;
        // Model replacement key or Swat voice mask according to behavior kind
        uint32 script_data;

        AI() : state(0), phase(0), script_register9(0), script_data(0)
        {
            uint32 index;
            for (index = 0; index < 10; ++index)
                parameters[index] = 0;
            for (index = 0; index < 6; ++index)
                work[index] = 0;
        }

        void SetStatePhase(uint32 value)
        {
            state = value;
            phase = value >> 16;
        }
    } ai;

    uint16 ReadRegister(uint32 index) const;
    void WriteRegister(uint32 index, uint32 value);
    void WriteAIParameter(uint32 index, uint32 value);
    uint16 ground_offset;
    uint16 sight_offset;
    Vector3 destination;
    void AllocateBehaviorTable(uint32 rows, uint32 columns);
    void FreeBehaviorTable();
    uint32 ActivateLinkedRecords();
    void SetBehaviorValue(uint32 row, uint32 column, uint32 value);
    uint32 GetBehaviorValue(uint32 row, uint32 column);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Finish();
    virtual void NoOp();
    virtual uint32 ExecuteCommand(uint32 command);
    virtual void SetParameters(uint32 command);
    virtual uint32 ResolveParameter(uint32 command);

protected:
    Enemy();
    static RecordCursor AlignScriptCursor(RecordCursor cursor);
    RecordCursor GetScriptLabel(uint32 index) const;
    void SetScriptLabel(uint32 index, const RecordCursor &script);
    uint32 ReadCommandWord();

private:
    uint32 ReadCommandValue();
    sint32 ReadSignedCommandValue();
    RecordCursor script_labels[8];
    uint32 GetBehaviorIndex(uint32 row, uint32 column) const;
    uint16 *behavior;
    uint8 behavior_rows;
    uint8 behavior_cols;
};

#endif
