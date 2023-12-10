-------------------------------- MODULE Plane --------------------------------

EXTENDS Integers
(*--algorithm wire
variables
 plane = 0,
 pilot = 0, \* outside, going to plane, inside
 chassi = 1,
 engine \in 0..1

define
stands == plane + chassi > 0
engine_work == plane = 1 => engine = 0
pilot_safe == pilot = 1 => engine = 0
end define;

fair process Engine \in 1..2
begin
EngineChange:
while (TRUE) do
 if (pilot /= 1 /\ engine = 0) then
    engine := 1
 elsif (plane /= 1 /\ engine = 1) then
    engine := 0
 else 
    skip;
 end if;
end while;
end process;

fair process Plane \in 1..2
begin
PlaneChange:
while (TRUE) do
 if (pilot /= 1 /\ plane = 0 /\ engine = 1) then
    plane := 1
 elsif (chassi = 1 /\ plane = 1 /\ pilot /= 1) then
    plane := 0
 else
    skip;   
 end if;
end while;
end process;

fair process Pilot \in 1..2
    variable pilotNeed = 1
begin
PilotChange:
while (TRUE) do
 if (pilot /= 1 /\ plane = 0 /\ engine = 0) then
    pilot := 1;
 elsif (pilot = 1 /\ plane = 0 /\ engine = 0) then
    pilot := pilotNeed;
    pilotNeed := 1 - pilotNeed
 elsif (pilot = 2 /\ plane = 1) then
    pilot := 0
 else
    skip;
 end if;
end while;
end process;

fair process Chassi \in 1..2
begin
ChassiChange:
while (TRUE) do
 if (chassi = 1 /\ plane = 1) then
    chassi := 0;
 else
    chassi := 1
 end if;
end while;
end process;

end algorithm;*)

\* BEGIN TRANSLATION
VARIABLES plane, pilot, chassi, engine, pc

(* define statement *)
stands == plane + chassi > 0
engine_work == plane = 1 => engine = 0
pilot_safe == pilot = 1 => engine = 0

VARIABLE pilotNeed

vars == << plane, pilot, chassi, engine, pc, pilotNeed >>

ProcSet == (1..2) \cup (1..2) \cup (1..2) \cup (1..2)

Init == (* Global variables *)
        /\ plane = 0
        /\ pilot = 0
        /\ chassi = 1
        /\ engine \in 0..1
        (* Process Pilot *)
        /\ pilotNeed = [self \in 1..2 |-> 1]
        /\ pc = [self \in ProcSet |-> CASE self \in 1..2 -> "EngineChange"
                                        [] self \in 1..2 -> "PlaneChange"
                                        [] self \in 1..2 -> "PilotChange"
                                        [] self \in 1..2 -> "ChassiChange"]

EngineChange(self) == /\ pc[self] = "EngineChange"
                      /\ IF (pilot /= 1 /\ engine = 0)
                            THEN /\ engine' = 1
                            ELSE /\ IF (plane /= 1 /\ engine = 1)
                                       THEN /\ engine' = 0
                                       ELSE /\ TRUE
                                            /\ UNCHANGED engine
                      /\ pc' = [pc EXCEPT ![self] = "EngineChange"]
                      /\ UNCHANGED << plane, pilot, chassi, pilotNeed >>

Engine(self) == EngineChange(self)

PlaneChange(self) == /\ pc[self] = "PlaneChange"
                     /\ IF (pilot /= 1 /\ plane = 0 /\ engine = 1)
                           THEN /\ plane' = 1
                           ELSE /\ IF (chassi = 1 /\ plane = 1 /\ pilot /= 1)
                                      THEN /\ plane' = 0
                                      ELSE /\ TRUE
                                           /\ plane' = plane
                     /\ pc' = [pc EXCEPT ![self] = "PlaneChange"]
                     /\ UNCHANGED << pilot, chassi, engine, pilotNeed >>

Plane(self) == PlaneChange(self)

PilotChange(self) == /\ pc[self] = "PilotChange"
                     /\ IF (pilot /= 1 /\ plane = 0 /\ engine = 0)
                           THEN /\ pilot' = 1
                                /\ UNCHANGED pilotNeed
                           ELSE /\ IF (pilot = 1 /\ plane = 0 /\ engine = 0)
                                      THEN /\ pilot' = pilotNeed[self]
                                           /\ pilotNeed' = [pilotNeed EXCEPT ![self] = 1 - pilotNeed[self]]
                                      ELSE /\ IF (pilot = 2 /\ plane = 1)
                                                 THEN /\ pilot' = 0
                                                 ELSE /\ TRUE
                                                      /\ pilot' = pilot
                                           /\ UNCHANGED pilotNeed
                     /\ pc' = [pc EXCEPT ![self] = "PilotChange"]
                     /\ UNCHANGED << plane, chassi, engine >>

Pilot(self) == PilotChange(self)

ChassiChange(self) == /\ pc[self] = "ChassiChange"
                      /\ IF (chassi = 1 /\ plane = 1)
                            THEN /\ chassi' = 0
                            ELSE /\ chassi' = 1
                      /\ pc' = [pc EXCEPT ![self] = "ChassiChange"]
                      /\ UNCHANGED << plane, pilot, engine, pilotNeed >>

Chassi(self) == ChassiChange(self)

Next == (\E self \in 1..2: Engine(self))
           \/ (\E self \in 1..2: Plane(self))
           \/ (\E self \in 1..2: Pilot(self))
           \/ (\E self \in 1..2: Chassi(self))
           \/ (* Disjunct to prevent deadlock on termination *)
              ((\A self \in ProcSet: pc[self] = "Done") /\ UNCHANGED vars)

Spec == /\ Init /\ [][Next]_vars
        /\ \A self \in 1..2 : WF_vars(Engine(self))
        /\ \A self \in 1..2 : WF_vars(Plane(self))
        /\ \A self \in 1..2 : WF_vars(Pilot(self))
        /\ \A self \in 1..2 : WF_vars(Chassi(self))

Termination == <>(\A self \in ProcSet: pc[self] = "Done")

\* END TRANSLATION
=============================================================================
\* Modification History
\* Last modified Sun Dec 10 18:38:53 MSK 2023 by mark
