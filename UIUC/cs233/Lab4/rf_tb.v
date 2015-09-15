   module test;
   reg       clk = 0, enable = 0, reset = 1;  // start by reseting the register file

   /* Make a regular pulsing clock with a 10 time unit period. */
   always #5 clk = !clk;

   reg [4:0] wr_regnum = 0, rd1_regnum = 0, rd2_regnum = 0;
   reg [31:0] wr_data = 0;
   wire [31:0] rd1_data, rd2_data;
   
   initial begin
      $dumpfile("rf.vcd");
      $dumpvars(0, test);
      # 10 reset = 0;
      // stop reseting the RF
      # 10 wr_regnum=10;wr_data=23;enable=1;
      # 10 wr_regnum=42;wr_data=521;      
      # 10 rd1_regnum=10; rd2_regnum=42;enable=0;  
      # 10 wr_regnum=10;wr_data=120; enable=1; //Should overwrite
      # 10 wr_regnum=0; wr_data=432; //Should NOT change
      # 10 rd1_regnum=10; rd2_regnum=0; enable=0;//Should be 120 and 0
      # 10 rd1_regnum=10;
      
      
      # 700 $finish;
   end
   
   initial begin
   end

   mips_regfile rf (rd1_data, rd2_data, rd1_regnum, rd2_regnum, 
                    wr_regnum, wr_data, enable, clk, reset);
   
endmodule // test
