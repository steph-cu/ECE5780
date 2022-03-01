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
   AP, BP, AC, BC, Rand_Dur, Rand_Dur2: Duration;
   Gen: Generator;
   Ran_Float: Float;
   Finish: Boolean;

   task type Buffer is
      entry Init;
      entry Receive(FI: in Integer);
      entry Send(FO: out Integer);
      entry Ready(R: out Boolean);
      entry Ready2(R2: out Boolean);
      entry Term;
   end Buffer;
   task body Buffer is
      type Section is array (0 .. 9) of Integer;
      Buf: Section;
      index: Integer;
      Ending: Boolean;
   begin
      Ending := false;
      loop
         select
            accept Init do
               index := 0;
            end Init;
         or
            accept Receive(FI: in Integer) do
               if index < 10 then
                  Buf(index) := FI;
                  index := index + 1;
               end if;
            end Receive;
         or
            accept Send(FO: out Integer) do
               if index /= 0 then
                  index := index - 1;
                  FO := Buf(index);
               else
                  FO := 0;
               end if;
            end Send;
         or
            accept Ready(R: out Boolean) do
               if index < 10 then
                  R := true;
               else
                  R := false;
               end if;
            end Ready;
         or
            accept Ready2(R2: out Boolean) do
               if index = 0 then
                  R2 := false;
               else
                  R2 := true;
               end if;
            end Ready2;
         or
            accept Term do
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
      type randRange is new Integer range 1 .. 25;
      package Rand_Int is new Ada.Numerics.Discrete_Random(randRange);
      use Rand_Int;
      G:Rand_Int.Generator;
      Ran: randRange;
      Buff_Ready, Ending: Boolean;
   begin
      Reset(G);
      Ending := false;
      loop
        select
            accept Init do
               B.Ready(Buff_Ready);
               if Buff_Ready then
                  Ran := random(G);
                  put("p - ");
                  put_line(randRange'Image(Ran));
               end if;
               B.Receive(Integer(Ran));
            end Init;
         or
            accept Term do
               Ending := true;
            end Term;
         or
            accept Check(Finish: out Boolean) do
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
            accept Init do
               Adder := 0;
            end Init;
         or
            accept Exiting do
               B.Send(FO);
               B.Ready2(Buff_Take_Ready);
               if Buff_Take_Ready then
                  put(" c -");
                  put_line(Integer'Image(FO));
                  Adder := Adder + FO;
               end if;
               if Adder >= 100 then
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
begin
   P.Init;
   C.Init;
   B.Init;
   Reset(Gen);
   AC := Ada.Calendar.Seconds(Ada.Calendar.Clock);
   BC := Ada.Calendar.Seconds(Ada.Calendar.Clock);
   AP := Ada.Calendar.Seconds(Ada.Calendar.Clock);
   BP := Ada.Calendar.Seconds(Ada.Calendar.Clock);
   loop
      Ran_Float := Random(Gen); -- part 5
      Rand_Dur := Duration(Ran_Float);
      exit when Rand_Dur < 1.0000;
   end loop;
   loop
      Ran_Float := Random(Gen); -- part 5
      Rand_Dur2 := Duration(Ran_Float);
      exit when Rand_Dur2 < 1.0000;
   end loop;
   BC := BC + Rand_Dur;
   BP := BP + Rand_Dur2;
   loop
      P.Check(Finish);
      AC := Ada.Calendar.Seconds(Ada.Calendar.Clock);
      AP := Ada.Calendar.Seconds(Ada.Calendar.Clock);
      if AC > BC then
         C.Exiting;
         loop
            Ran_Float := Random(Gen); -- part 5
            Rand_Dur := Duration(Ran_Float);
            exit when Rand_Dur < 1.0000;
         end loop;
         BC := BC + Rand_Dur;
      end if;
      if AP > BP then
         P.Init;
         loop
            Ran_Float := Random(Gen); -- part 5
            Rand_Dur2 := Duration(Ran_Float);
            exit when Rand_Dur2 < 1.0000;
         end loop;
         BP := BP + Rand_Dur2;
      end if;
      exit when Finish = true;
   end loop;
end Part3_skeleton;
