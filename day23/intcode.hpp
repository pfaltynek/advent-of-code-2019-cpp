#ifndef INTCODE_HPP
#define INTCODE_HPP

#include <iostream>
#include <queue>
#include <stdint.h>
#include <vector>

class int_code {
  public:
	int64_t get_address() const;
	bool simulate_intcode();
	void reset();
	int_code(int64_t address, std::vector<int64_t>& ints);
	bool get_outgoing_packet(int64_t& address, int64_t& x, int64_t& y);
	void set_incoming_packet(int64_t x, int64_t y);
	int_code(){};

  private:
	typedef enum OPCODES {
		OP_ADD = 1,
		OP_MUL = 2,
		OP_INP = 3,
		OP_OUT = 4,
		OP_JNZ = 5,
		OP_JZ = 6,
		OP_LES = 7,
		OP_EQL = 8,
		OP_SET_REL_BASE = 9,
		OP_FIN = 99
	} opcodes_t;

	int64_t address_;
	uint64_t idx_;
	std::vector<int64_t> ints_;
	std::queue<int64_t> input_;
	std::queue<int64_t> output_;
	int64_t relative_base_;

	bool get_int(const uint64_t idx, const int32_t param_mode, int64_t& value);
	bool set_int(const uint64_t idx, const int32_t param_mode, const int64_t value);
	void safe_memory(uint64_t idx);
};

int64_t int_code::get_address() const {
	return address_;
}

int_code::int_code(int64_t address, std::vector<int64_t>& ints) {
	idx_ = 0;
	relative_base_ = 0;
	address_ = address;
	ints_ = ints;
	input_.push(address);
}

void int_code::reset() {
}

void int_code::safe_memory(uint64_t idx) {
	while (idx >= ints_.size()) {
		ints_.push_back(0);
	}
}

bool int_code::get_int(const uint64_t idx, const int32_t param_mode, int64_t& value) {
	switch (param_mode) {
		case 2:
			safe_memory(relative_base_ + idx);
			value = ints_[relative_base_ + idx];
			break;
		case 1:
			value = idx;
			break;
		case 0:
			safe_memory(idx);
			value = ints_[idx];
			break;
		default:
			return false;
			break;
	}

	return true;
}

bool int_code::set_int(const uint64_t idx, const int32_t param_mode, const int64_t value) {
	switch (param_mode) {
		case 2:
			safe_memory(relative_base_ + idx);
			ints_[relative_base_ + idx] = value;
			break;
		case 1:
			return false;
			break;
		case 0:
			safe_memory(idx);
			ints_[idx] = value;
			break;
		default:
			return false;
			break;
	}

	return true;
}

bool int_code::simulate_intcode() {
	int64_t op1, op2;
	int64_t opcode, param_modes, param_mode[3];
	bool result = false;

	opcode = ints_[idx_] % 100;
	param_modes = ints_[idx_] / 100;

	for (uint64_t i = 0; i < 3; i++) {
		param_mode[i] = param_modes % 10;
		param_modes /= 10;
	}

	switch (static_cast<opcodes_t>(opcode)) {
		case opcodes_t::OP_ADD:
			get_int(ints_[idx_ + 1], param_mode[0], op1);
			get_int(ints_[idx_ + 2], param_mode[1], op2);
			set_int(ints_[idx_ + 3], param_mode[2], op1 + op2);
			idx_ += 4;
			break;
		case opcodes_t::OP_MUL:
			get_int(ints_[idx_ + 1], param_mode[0], op1);
			get_int(ints_[idx_ + 2], param_mode[1], op2);
			set_int(ints_[idx_ + 3], param_mode[2], op1 * op2);
			idx_ += 4;
			break;
		case opcodes_t::OP_INP:
			if (input_.size() > 0) {
				op1 = input_.front();
				input_.pop();
			} else {
				op1 = -1;
			}
			set_int(ints_[idx_ + 1], param_mode[0], op1);
			idx_ += 2;
			break;
		case opcodes_t::OP_OUT:
			get_int(ints_[idx_ + 1], param_mode[0], op1);
			output_.push(op1);
			idx_ += 2;
			break;
		case opcodes_t::OP_JNZ:
			get_int(ints_[idx_ + 1], param_mode[0], op1);
			if (op1) {
				get_int(ints_[idx_ + 2], param_mode[1], op2);
				idx_ = op2;
			} else {
				idx_ += 3;
			}
			break;
		case opcodes_t::OP_JZ:
			get_int(ints_[idx_ + 1], param_mode[0], op1);
			if (!op1) {
				get_int(ints_[idx_ + 2], param_mode[1], op2);
				idx_ = op2;
			} else {
				idx_ += 3;
			}
			break;
		case opcodes_t::OP_LES:
			get_int(ints_[idx_ + 1], param_mode[0], op1);
			get_int(ints_[idx_ + 2], param_mode[1], op2);
			set_int(ints_[idx_ + 3], param_mode[2], op1 < op2 ? 1 : 0);
			idx_ += 4;
			break;
		case opcodes_t::OP_EQL:
			get_int(ints_[idx_ + 1], param_mode[0], op1);
			get_int(ints_[idx_ + 2], param_mode[1], op2);
			set_int(ints_[idx_ + 3], param_mode[2], op1 == op2 ? 1 : 0);
			idx_ += 4;
			break;
		case opcodes_t::OP_SET_REL_BASE:
			get_int(ints_[idx_ + 1], param_mode[0], op1);
			relative_base_ += op1;
			idx_ += 2;
			break;
		case opcodes_t::OP_FIN:
			result = true;
			break;
		default:
			return false;
	}

	return result;
}

bool int_code::get_outgoing_packet(int64_t& address, int64_t& x, int64_t& y) {
	if (output_.size() >= 3) {
		address = output_.front();
		output_.pop();
		x = output_.front();
		output_.pop();
		y = output_.front();
		output_.pop();

		//std::cout << "Sent [" << address_ << "]->[" << address << "] " << x << ", " << y << std::endl;

		return true;
	} else {
		return false;
	}
}

void int_code::set_incoming_packet(int64_t x, int64_t y) {
	input_.push(x);
	input_.push(y);

	//std::cout << "Received [" << address_ << "] " << x << ", " << y << std::endl;
}
#endif // INTCODE_HPP