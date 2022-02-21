with Ada.Text_IO;
use  Ada.Text_IO;
with Text_Io;
use  Text_Io;
with Ada.Calendar;
use  Ada.Calendar;
--with Ada.Numerics.Discrete_Random;
--use Ada.Numerics.Discrete_Random;
with Ada.Numerics.Float_Random;
use Ada.Numerics.Float_Random;

-- the main begin is sequential and each procedure is
-- sequential (called once each), but different procedures occur concurrently with the begin
procedure Part2_Skeleton  is

   vTime, F1_Start, F1_Curr, F2_Start, F2_Curr, F3_Start, F3_Curr, F3_Wait, Before, After: Duration;
   Rand_Dur: Duration;
   --subtype LFloat is Float range 0.2000 .. 0.8000;
   --Rand: LFloat; -- part 5
   Rand: Float;
   G: Generator; -- part 5
   Res: Boolean;

   package DIO is new Text_Io.Fixed_Io(Duration); --To print Duration variables you can instantiate the generic
						  --package Text_Io.Fixed_Io with a duration type:
						  --"package DIO is new Text_Io.Fixed_Io(Duration);"
						  --The DIO package will then export, among other things,
						  --the procedure DIO.Put(D:Duration, Fore:Field, Aft:Field)
						  --to print variable D of type Duration. See an example
						  --on how to use this below.

   task type Watchdog is
      entry Init;
      entry ReSyn(Ending: Duration);
   end Watchdog;
   task body Watchdog is
      --Declarations
      WatchF3: Duration;
      --Finish: Boolean;
   begin
      -- Accepts
      -- Starts the Watchdog
      loop
         select
            accept Init do
               WatchF3 := Ada.Calendar.Seconds(Ada.Calendar.Clock);
            end Init;
         -- ReSynch the Loop
         or
            accept ReSyn(Ending: in Duration) do
               if Ending - WatchF3 > 0.5000 then
                  Put_Line("");-- Add a new line
                  Put_Line("WARNING: F3 did not finish on time, reseting");
                  Put_Line("");
                  Res := True;
               else
                  Res := False;
               end if;
            end Resyn;
         end select;
      end loop;
   end Watchdog;

   --Declare F3, which prints out a message when it starts and stops executing
   procedure F3(Currtime: Duration; StartF3: Duration; FinishF3: Duration) is
   begin
      if StartF3 = 0.0 and then FinishF3 = 0.0 then
         Put_Line(""); --Add a new line
         Put_Line("F3 has started executing. The time is now:");
         DIO.Put(Currtime);
      else
         Put_Line("");
         Put_Line("F3 has finished executing. The time is now:");
         DIO.Put(Currtime + (FinishF3 - StartF3)); --Needed since time starts at 0 and FinishF1 and StartF1 are not virtual times
      end if;
   end F3;

   --Declare F2, which prints out a message when it starts and stops executing
   procedure F2(Currtime: Duration; StartF2: Duration; FinishF2: Duration) is
   begin
      if StartF2 = 0.0 and then FinishF2 = 0.0 then
         Put_Line(""); --Add a new line
         Put_Line("F2 has started executing. The time is now:");
         DIO.Put(Currtime);
      else
         Put_Line("");
         Put_Line("F2 has finished executing. The time is now:");
         DIO.Put(Currtime + (FinishF2 - StartF2)); --Needed since time starts at 0 and FinishF1 and StartF1 are not virtual times
      end if;
   end F2;

    --Declare F1, which prints out a message when it starts and stops executing
   procedure F1(Currtime: Duration; StartF1: Duration; FinishF1: Duration) is
   begin
      if StartF1 = 0.0 and then FinishF1 = 0.0 then
         Put_Line(""); --Add a new line
         Put_Line("F1 has started executing. The time is now:");
         DIO.Put(Currtime);
      else
         Put_Line("");
         Put_Line("F1 has finished executing. The time is now:");
         DIO.Put(Currtime + (FinishF1 - StartF1)); --Needed since time starts at 0 and FinishF1 and StartF1 are not virtual times
      end if;
   end F1;
   WatchdogF3: Watchdog;

-- The beginning of the procedure
begin
   vTime := 0.0;
   Before := Ada.Calendar.Seconds(Ada.Calendar.Clock);
   Res := False;
   --Put_Line("We are moving through");
   Reset(G); -- part 5
   loop
      Rand := Random(G); -- part 5
      Rand_Dur := Duration(Rand);
      exit when Rand_Dur < 1.0000;
   end loop;
   --Text_IO.Put_Line("Made it to Part1");

   --Main loop
   loop
      After := Ada.Calendar.Seconds(Ada.Calendar.Clock);
      --Put_Line("We are moving through");

      --Execute F1 every 1 second
      if After - Before >= 1.000000000 and then Res = False then

         vTime := vTime + (After - Before); --Needed since time starts at 0

         -- F1 section
         F1_Start := Ada.Calendar.Seconds(Ada.Calendar.Clock); --Get start time of F1
         F1(Currtime => vTime, StartF1 => 0.0, FinishF1 => 0.0); --Initialize F1
         loop --F1 starts

            --Get current time
            F1_Curr := Ada.Calendar.Seconds(Ada.Calendar.Clock);

            exit when  F1_Curr - F1_Start >= 0.3000; --Assuming F1 takes 0.3 seconds

         end loop; --F1 ends
         --After F1 finishes executing, call the F1 procedure again to obtain the finish time
         F1(Currtime => vTime, StartF1 => F1_Start, FinishF1 => F1_Curr);

         -- F2 section
         F2_Start := vTime + F1_Curr - F1_Start;
         F2(Currtime => F2_Start, StartF2 => 0.0, FinishF2 => 0.0);
         loop -- F2 starts
            F2_Curr := Ada.Calendar.Seconds(Ada.Calendar.Clock);

            exit when F2_Curr - F1_Curr >= 0.1500;
         end loop;--F2 ends
         F2(Currtime => F2_Start, StartF2 => F1_Curr, FinishF2 => F2_Curr);

         -- F3 section
         -- Waiting to start .5 seconds after F1
         loop
            F3_Wait :=  Ada.Calendar.Seconds(Ada.Calendar.Clock);
            exit when F3_Wait -F1_Start >= 0.5000;
         end loop;
         F3_Start := Ada.Calendar.Seconds(Ada.Calendar.Clock);

         -- start watchdog
         WatchdogF3.Init;

         -- Start F3
         F3(Currtime => vTime + F3_Start - F1_Start, StartF3 => 0.0, FinishF3 => 0.0);
         loop
            F3_Curr :=  Ada.Calendar.Seconds(Ada.Calendar.Clock);
            exit when F3_Curr - F3_Start >= Rand_Dur;
         end loop;
         F3(Currtime => vTime + F3_Start - F1_Start, StartF3 => F3_Start, FinishF3 => F3_Curr);

         WatchdogF3.ReSyn(F3_Curr);

         loop
            Rand := Random(G); -- part 5
            Rand_Dur := Duration(Rand);
            exit when Rand_Dur < 1.0000;
         end loop;
         Rand_Dur := Duration(Rand);

         Before := After;-- Reset before

      elsif Res = True then
         Before := After + 1.0000; -- Reset for a little later
         vTime := vTime + 1.0000;
         Res := False;
      end if; --Every 1 second

   end loop; --Main loop

  end Part2_Skeleton;
