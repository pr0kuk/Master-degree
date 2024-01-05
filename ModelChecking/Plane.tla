-------------------------------- MODULE Plane --------------------------------

EXTENDS Integers
(*--algorithm Plane
variables
 fly = FALSE, 
 pilot = FALSE,
 chassi = TRUE,
 fuel = FALSE,
 cleaned = FALSE,
 autopilot = FALSE,
 up = TRUE

define
stands == ~fly => chassi
enough_fuel == ~fly \/ fuel
clean_check == pilot => cleaned
somebody_control == fly => (pilot \/ autopilot)
end define;

fair+ process Staff = "Staff"
begin StaffChange:
while (TRUE) do
 either
  if (~fly /\ ~fuel) then
     fuel := TRUE
  else 
     skip;
  end if;
 or
  if (~fly /\ ~cleaned) then
     cleaned := TRUE
  else 
     skip;
  end if;
 end either;
end while;
end process;

fair+ process Pilot = "Pilot"
begin PilotChange:
while (TRUE) do
 if (~pilot /\ ~fly /\ cleaned) then
    pilot := TRUE
 elsif (pilot /\ ~fly /\ fuel) then
    fly := TRUE
 elsif (fly /\ chassi /\ ~up /\ (pilot \/ autopilot)) then \* sit down
    fly := FALSE || pilot := FALSE || fuel := FALSE || cleaned := FALSE || up := TRUE || autopilot := FALSE
 else
    skip;
 end if;
end while;
end process;

fair+ process Flight = "Flight"
begin FlightChange:
while (TRUE) do
 either
  if ((pilot \/ autopilot) /\ fly /\ chassi /\ up) then
     chassi := FALSE || up := FALSE
  elsif ((pilot \/ autopilot) /\ fly /\ ~chassi /\ ~up) then
     chassi := TRUE
  else
     skip;
  end if;
 or
  if ((pilot \/ autopilot) /\ fly /\ chassi /\ up) then
     up := FALSE
  elsif ((pilot \/ autopilot) /\ fly /\ ~chassi /\ ~up) then
     chassi := TRUE
  else
     skip;
  end if;
 end either;
end while;
end process;

fair+ process AutoPilot = "AutoPilot"
begin AutoPilotChange:
while TRUE do
 if (pilot /\ fly) then
    autopilot := TRUE
 elsif (autopilot /\ fly /\ pilot) then
    pilot := FALSE
 else
    skip;
 end if;
end while;
end process;

end algorithm;*)

\* BEGIN TRANSLATION
VARIABLES fly, pilot, chassi, fuel, cleaned, autopilot, up, pc

(* define statement *)
stands == ~fly => chassi
enough_fuel == ~fly \/ fuel
clean_check == pilot => cleaned
somebody_control == fly => (pilot \/ autopilot)


vars == << fly, pilot, chassi, fuel, cleaned, autopilot, up, pc >>

ProcSet == {"Staff"} \cup {"Pilot"} \cup {"Flight"} \cup {"AutoPilot"}

Init == (* Global variables *)
        /\ fly = FALSE
        /\ pilot = FALSE
        /\ chassi = TRUE
        /\ fuel = FALSE
        /\ cleaned = FALSE
        /\ autopilot = FALSE
        /\ up = TRUE
        /\ pc = [self \in ProcSet |-> CASE self = "Staff" -> "StaffChange"
                                        [] self = "Pilot" -> "PilotChange"
                                        [] self = "Flight" -> "FlightChange"
                                        [] self = "AutoPilot" -> "AutoPilotChange"]

StaffChange == /\ pc["Staff"] = "StaffChange"
               /\ \/ /\ IF (~fly /\ ~fuel)
                           THEN /\ fuel' = TRUE
                           ELSE /\ TRUE
                                /\ fuel' = fuel
                     /\ UNCHANGED cleaned
                  \/ /\ IF (~fly /\ ~cleaned)
                           THEN /\ cleaned' = TRUE
                           ELSE /\ TRUE
                                /\ UNCHANGED cleaned
                     /\ fuel' = fuel
               /\ pc' = [pc EXCEPT !["Staff"] = "StaffChange"]
               /\ UNCHANGED << fly, pilot, chassi, autopilot, up >>

Staff == StaffChange

PilotChange == /\ pc["Pilot"] = "PilotChange"
               /\ IF (~pilot /\ ~fly /\ cleaned)
                     THEN /\ pilot' = TRUE
                          /\ UNCHANGED << fly, fuel, cleaned, autopilot, up >>
                     ELSE /\ IF (pilot /\ ~fly /\ fuel)
                                THEN /\ fly' = TRUE
                                     /\ UNCHANGED << pilot, fuel, cleaned, 
                                                     autopilot, up >>
                                ELSE /\ IF (fly /\ chassi /\ ~up /\ (pilot \/ autopilot))
                                           THEN /\ /\ autopilot' = FALSE
                                                   /\ cleaned' = FALSE
                                                   /\ fly' = FALSE
                                                   /\ fuel' = FALSE
                                                   /\ pilot' = FALSE
                                                   /\ up' = TRUE
                                           ELSE /\ TRUE
                                                /\ UNCHANGED << fly, pilot, 
                                                                fuel, cleaned, 
                                                                autopilot, up >>
               /\ pc' = [pc EXCEPT !["Pilot"] = "PilotChange"]
               /\ UNCHANGED chassi

Pilot == PilotChange

FlightChange == /\ pc["Flight"] = "FlightChange"
                /\ \/ /\ IF ((pilot \/ autopilot) /\ fly /\ chassi /\ up)
                            THEN /\ /\ chassi' = FALSE
                                    /\ up' = FALSE
                            ELSE /\ IF ((pilot \/ autopilot) /\ fly /\ ~chassi /\ ~up)
                                       THEN /\ chassi' = TRUE
                                       ELSE /\ TRUE
                                            /\ UNCHANGED chassi
                                 /\ up' = up
                   \/ /\ IF ((pilot \/ autopilot) /\ fly /\ chassi /\ up)
                            THEN /\ up' = FALSE
                                 /\ UNCHANGED chassi
                            ELSE /\ IF ((pilot \/ autopilot) /\ fly /\ ~chassi /\ ~up)
                                       THEN /\ chassi' = TRUE
                                       ELSE /\ TRUE
                                            /\ UNCHANGED chassi
                                 /\ up' = up
                /\ pc' = [pc EXCEPT !["Flight"] = "FlightChange"]
                /\ UNCHANGED << fly, pilot, fuel, cleaned, autopilot >>

Flight == FlightChange

AutoPilotChange == /\ pc["AutoPilot"] = "AutoPilotChange"
                   /\ IF (pilot /\ fly)
                         THEN /\ autopilot' = TRUE
                              /\ pilot' = pilot
                         ELSE /\ IF (autopilot /\ fly /\ pilot)
                                    THEN /\ pilot' = FALSE
                                    ELSE /\ TRUE
                                         /\ pilot' = pilot
                              /\ UNCHANGED autopilot
                   /\ pc' = [pc EXCEPT !["AutoPilot"] = "AutoPilotChange"]
                   /\ UNCHANGED << fly, chassi, fuel, cleaned, up >>

AutoPilot == AutoPilotChange

Next == Staff \/ Pilot \/ Flight \/ AutoPilot

Spec == /\ Init /\ [][Next]_vars
        /\ SF_vars(Staff)
        /\ SF_vars(Pilot)
        /\ SF_vars(Flight)
        /\ SF_vars(AutoPilot)

\* END TRANSLATION

start_fly == [](pilot ~> fly)
end_fly == [](fly ~> ~fly)
autopilot_sometimes == <>(autopilot)
=============================================================================
\* Modification History
\* Last modified Fri Dec 15 21:28:57 MSK 2023 by mark
