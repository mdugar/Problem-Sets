module mips_decode(ALUOp, RegWrite, BEQ, LARGEST, ALUSrc, MemRead, MemWrite, MemToReg, RegDst, 
		   opcode, funct);

   output [2:0] ALUOp;
   output 	RegWrite, BEQ, LARGEST, ALUSrc, MemRead, MemWrite, MemToReg, RegDst;
   input  [5:0] opcode, funct;

   given_decode given(ALUOp, RegWrite, BEQ, ALUSrc, MemRead, MemWrite, MemToReg, RegDst, 
		opcode, funct);
   assign LARGEST = (opcode==6'h00 & funct==6'h38);
   
endmodule // mips_decode

module machine(clk, reset);
   input        clk, reset;

   wire [31:0] 	PC;
   wire [31:2]  next_PC, PC_plus4, PC_target;
   wire [31:0] 	inst;
   
   wire [31:0] 	imm = {{ 16{inst[15]} }, inst[15:0] };  // sign-extended immediate
   wire [4:0] 	rs = inst[25:21];
   wire [4:0] 	rt = inst[20:16];
   wire [4:0] 	rd = inst[15:11];
   wire [5:0] 	opcode = inst[31:26];
   wire [5:0] 	funct = inst[5:0];

   wire [4:0] 	wr_regnum;
   wire [2:0] 	ALUOp_ORIGINAL, ALUOp;

   wire 	RegWrite, BEQ, LARGEST, ALUSrc, MemRead, MemWrite, MemToReg, RegDst;
   wire         PCSrc, zero, negative;
   wire [31:0] 	rd1_data, rd2_data, B_data, alu_out_data, load_data, wr_data_ORIGINAL, wr_data;

   wire [31:0] 	greater_out;
   


   register #(30, 30'h100000) PC_reg(PC[31:2], next_PC[31:2], clk, /* enable */1'b1, reset);
   assign PC[1:0] = 2'b0;  // bottom bits hard coded to 00
   adder30 next_PC_adder(PC_plus4, PC[31:2], 30'h1);
   adder30 target_PC_adder(PC_target, PC_plus4, imm[29:0]);
   mux2v #(30) branch_mux(next_PC, PC_plus4, PC_target, PCSrc);
   assign PCSrc = BEQ & zero;
      
   instruction_memory imem (PC[31:2], inst);

   mips_decode decode(ALUOp_ORIGINAL, RegWrite, BEQ, LARGEST, ALUSrc, MemRead, MemWrite, MemToReg, RegDst, 
		      opcode, funct);
   
   regfile rf (rs, rt, wr_regnum, 
               rd1_data, rd2_data, wr_data, 
               RegWrite, clk, reset);

 

   mux2v #(32) imm_mux(B_data, rd2_data, imm, ALUSrc);

   mux2v #(3) alu_LARGEST_mux(ALUOp, ALUOp_ORIGINAL, 3'h1, LARGEST);
   alu32 alu(alu_out_data, zero, negative, ALUOp, rd1_data, B_data);
   
   data_mem data_memory(load_data, alu_out_data, rd2_data, MemRead, MemWrite, clk, reset);
   
   mux2v #(32) wb_mux(wr_data_ORIGINAL, alu_out_data, load_data, MemToReg);
   mux2v #(5) rd_mux(wr_regnum, rt, rd, RegDst);

   mux2v #(32) greater_of_two_mux(greater_out, rd1_data, rd2_data, negative);
   mux2v #(32) write_larger_mux(wr_data, wr_data_ORIGINAL, greater_out, LARGEST);
   
endmodule // machine
