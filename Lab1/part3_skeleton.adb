with Ada.Text_IO;
use  Ada.Text_IO;
with Text_Io;
use  Text_Io;
with Ada.Calendar;
use  Ada.Calendar;
with Ada.Numerics.Discrete_Random;
with Ada.Numerics.Float_Random;
use Ada.Numerics.Float_Random;

procedure Part3_skeleton is
   --Variable declaration
   AP, BP, AC, BC, Rand_Dur, Rand_Dur2: Duration;
   Gen: Generator;
   Ran_Float: Float;
   Finish: Boolean;

   task type Buffer is
      entry Init;
      entry Receive(FI: in Integer); --take in a integer
      entry Send(FO: out Integer); --send an integer
      entry Ready(R: out Boolean); --ready signal for producer
      entry Ready2(R2: out Boolean); --ready signal for consumer
      entry Term;  
   end Buffer;
   task body Buffer is
      type Section is array (0 .. 9) of Integer;
      Buf: Section;
      index: Integer;
      Ending: Boolean;
   begin
      Ending := false; --If the program is going to end
      loop
         select -- if the program is starting set the index to 0
            accept Init do
               index := 0;
            end Init;
         or
            accept Receive(FI: in Integer) do -- geting an integer from the producer
               if index < 10 then -- if the buffer has space take it in
                  Buf(index) := FI; 
                  index := index + 1;
               end if;
            end Receive;
         or
            accept Send(FO: out Integer) do -- sending an integer to the consumer
               if index /= 0 then -- if the buffer has something in it
                  index := index - 1;
                  FO := Buf(index);
               else -- if the buffer is empty
                  FO := 0;
               end if;
            end Send;
         or
            accept Ready(R: out Boolean) do -- to tell either the producer it's avalible to get a number
               if index < 10 then
                  R := true;
               else
                  R := false;
               end if;
            end Ready;
         or
            accept Ready2(R2: out Boolean) do -- to let the consumer it's avaliable to get a number
               if index = 0 then
                  R2 := false;
               else
                  R2 := true;
               end if;
            end Ready2;
         or
            accept Term do -- to start the ending cycle
               Ending := true;
            end Term;
         end select;
         exit when Ending = true;
      end loop;
   end Buffer;
   B: Buffer;

   task type Producer is
      entry Init;
      entry Term;
      entry Check(Finish: out Boolean);
   end Producer;
   task body Producer is
      --variable declaration and instantiation 
      type randRange is new Integer range 1 .. 25;
      package Rand_Int is new Ada.Numerics.Discrete_Random(randRange);
      use Rand_Int;
      G:Rand_Int.Generator;
      Ran: randRange;
      Buff_Ready, Ending: Boolean;
   begin
      Reset(G); -- set up the random number generator
      Ending := false; -- set up so that the program isn't ending
      loop
        select
            accept Init do
               B.Ready(Buff_Ready); -- check to see if the buffer is ready
               if Buff_Ready then -- if it is generate a random number
                  Ran := random(G); 
                  put("p - ");
                  put_line(randRange'Image(Ran));
               end if;
               B.Receive(Integer(Ran)); -- send the number to the buffer
            end Init;
         or
            accept Term do -- setting the end to true to end the program
               Ending := true;
            end Term;
         or
            accept Check(Finish: out Boolean) do -- check to see if the program is supposed to end
               Finish := Ending;
            end Check;
         end select;
         exit when Ending = true;
      end loop;
   end Producer;
   P: Producer;

   task type Consumer is   -- needs a way to keep from taking when empty
      entry Init;
      entry Exiting;
   end Consumer;
   task body Consumer is
      Adder: Integer;
      FO: Integer;
      Buff_Take_Ready: Boolean;
   begin
      loop
         select
            accept Init do -- set up the adder to 0
               Adder := 0;
            end Init;
         or
            accept Exiting do
               B.Send(FO);
               B.Ready2(Buff_Take_Ready); -- check if the buffer is ready.
               if Buff_Take_Ready then -- if it's ready, then take a number, print it, and add it.
                  put(" c -");
                  put_line(Integer'Image(FO));
                  Adder := Adder + FO;
               end if;
               if Adder >= 100 then -- if the adder is above 100 then end the program.
                  put_line("We are done:");
                  put_line(Integer'Image(Adder));
                  P.Term;
                  B.Term;
               end if;
            end Exiting;
         end select;
      end loop;
   end Consumer;
   C: Consumer;
begin --begin the procedure
   -- initialize the producer, consumer, and buffer
   P.Init;
   C.Init;
   B.Init;
   Reset(Gen);
   AC := Ada.Calendar.Seconds(Ada.Calendar.Clock);
   BC := Ada.Calendar.Seconds(Ada.Calendar.Clock);
   AP := Ada.Calendar.Seconds(Ada.Calendar.Clock);
   BP := Ada.Calendar.Seconds(Ada.Calendar.Clock);
   loop -- wait a random duration for the consumer
      Ran_Float := Random(Gen); -- part 5
      Rand_Dur := Duration(Ran_Float);
      exit when Rand_Dur < 1.0000;
   end loop;
   loop -- wait a random duration for the producer
      Ran_Float := Random(Gen);
      Rand_Dur2 := Duration(Ran_Float);
      exit when Rand_Dur2 < 1.0000;
   end loop;
   BC := BC + Rand_Dur;
   BP := BP + Rand_Dur2;
   loop
      P.Check(Finish); -- check the producer
      AC := Ada.Calendar.Seconds(Ada.Calendar.Clock);
      AP := Ada.Calendar.Seconds(Ada.Calendar.Clock);
      if AC > BC then -- if you need to execute the consumer
         C.Exiting;
         loop
            Ran_Float := Random(Gen);
            Rand_Dur := Duration(Ran_Float);
            exit when Rand_Dur < 1.0000;
         end loop;
         BC := BC + Rand_Dur;
      end if;
      if AP > BP then -- if you need to execute the producer
         P.Init;
         loop
            Ran_Float := Random(Gen); 
            Rand_Dur2 := Duration(Ran_Float);
            exit when Rand_Dur2 < 1.0000;
         end loop;
         BP := BP + Rand_Dur2;
      end if;
      exit when Finish = true;
   end loop;
end Part3_skeleton;
