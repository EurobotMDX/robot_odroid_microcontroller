#include <Cth.h>

#define _CI_MAX_COMMAND_SEQUENCE_LEN 100

extern void move_gripper_jaw(const int jaw_id, const float new_angle);
extern void enable_grippers();
extern void disable_grippers();

class CommandInterpreter
{
  public:
    CommandInterpreter();

    void initialize(const char start_char, const char end_char, const char split_char);
    void destroy();

    void parse_sequence(String sequence);
    bool execute_next_instruction();

  private:
    char _start_char;
    char _end_char;
    char _split_char;
    
    bool _CI_delay_latch = false;

    String _buffer;
    const int _find(const String &sequence, const char c, const int start_index);
    bool _get_next_instruction(const String &sequence_in, String &instruction_out, int &start_index, int &end_index);

    bool _execute_instruction(const String &instruction);

    String _get_variable_as_string(const String &instruction, const int position);
    float _get_variable_as_float(const String &instruction, const int position);
    int _get_variable_as_int(const String &instruction, const int position);
};


CommandInterpreter::CommandInterpreter()
{
}

void CommandInterpreter::initialize(const char start_char='{', const char end_char='}', const char split_char=',')
{
  _start_char = start_char;
  _end_char = end_char;
  _split_char = split_char;

  _CI_delay_latch = false;
}

void CommandInterpreter::parse_sequence(String sequence)
{
  sequence.trim();
  sequence.toLowerCase();

  // TODO: fix the line below because it is a potential crash point  --- ask Chibuike :)
  String string_buffer = _buffer + sequence;
  
  int start_index = constrain(((int)string_buffer.length()) - _CI_MAX_COMMAND_SEQUENCE_LEN, 0, _CI_MAX_COMMAND_SEQUENCE_LEN);
  _buffer = string_buffer.substring(start_index);

//  Serial.print("seq: ");
//  Serial.println(sequence);
//  Serial.print("_buffer: ");
//  Serial.println(_buffer);
}

bool CommandInterpreter::execute_next_instruction()
{
  if (_CI_delay_latch)
  {
    return false;
  }

  String instruction = "";
  int start_index    = -1;
  int end_index      = -1;

  if ( _get_next_instruction(_buffer, instruction, start_index, end_index) )
  {
    _buffer = _buffer.substring(end_index + 1);

    return _execute_instruction(instruction);
  }
  else if (start_index >= 0)
  {
    _buffer = _buffer.substring(start_index);
  }

  return false;
}

const int CommandInterpreter::_find(const String &sequence, const char c, const int start_index)
{
  int index = constrain(start_index, 0, sequence.length());
  const int n = sequence.length();

  while (index < n)
  {
    if (((char)sequence.charAt(index)) == c)
    {
      return index;
    }

    index += 1;
  }

  // returns -1 if cannot find the given character
  return -1;
}

bool CommandInterpreter::_get_next_instruction(const String &sequence_in, String &instruction_out, int &start_index, int &end_index)
{
  start_index = _find(sequence_in, '{', 0);
  end_index   = _find(sequence_in, '}', start_index);
  instruction_out = sequence_in.substring(start_index, end_index + 1);

  return (start_index >= 0) && (end_index >= 0);
}

String CommandInterpreter::_get_variable_as_string(const String &instruction, const int position)
{
  int index = 0;
  int start_index = 1;
  int end_index = _find(instruction, ',', start_index);

  while (index < position)
  {
    start_index = end_index + 1;
    end_index = _find(instruction, ',', start_index);

    if (end_index < 0)
    {
      end_index = instruction.length() - 1;
    }

    index += 1;
  }

  return instruction.substring(start_index, end_index);
}

float CommandInterpreter::_get_variable_as_float(const String &instruction, const int position)
{
  return _get_variable_as_string(instruction, position).toFloat();
}

int CommandInterpreter::_get_variable_as_int(const String &instruction, const int position)
{
  return _get_variable_as_string(instruction, position).toInt();
}

bool CommandInterpreter::_execute_instruction(const String &instruction)
{
  const String cmd = _get_variable_as_string(instruction, 0);

  if (cmd == "s")
  {
    const int servo_id = _get_variable_as_int(instruction, 1);
    const float angle = _get_variable_as_float(instruction, 2);

    move_gripper_jaw(servo_id, angle);
  }
  else if (cmd == "d")
  {
    const int delay_time = _get_variable_as_int(instruction, 1);
    
//    Serial.print("Waiting for ");
//    Serial.print(delay_time);
//    Serial.println(" msecs");

    _CI_delay_latch = true;
    Scheduler.delay(delay_time);
    _CI_delay_latch = false;
  }
  else if (cmd == "o")
  {
    const int state = _get_variable_as_int(instruction, 1);

    switch(state)
    {
      case 1:
//        Serial.println("Grippers enabled");
        enable_grippers();
        break;
      default:
//        Serial.println("Grippers disabled");
        disable_grippers();
    }
  }

  return true;
}

