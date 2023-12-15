---- MODULE MC ----
EXTENDS Plane, TLC

\* SPECIFICATION definition @modelBehaviorSpec:0
spec_1702664941383157000 ==
Spec
----
\* INVARIANT definition @modelCorrectnessInvariants:0
inv_1702664941393158000 ==
stands
----
\* INVARIANT definition @modelCorrectnessInvariants:1
inv_1702664941403159000 ==
enough_fuel
----
\* INVARIANT definition @modelCorrectnessInvariants:2
inv_1702664941413160000 ==
clean_check
----
\* INVARIANT definition @modelCorrectnessInvariants:3
inv_1702664941424161000 ==
somebody_control
----
\* PROPERTY definition @modelCorrectnessProperties:0
prop_1702664941434162000 ==
start_fly
----
\* PROPERTY definition @modelCorrectnessProperties:1
prop_1702664941445163000 ==
end_fly
----
\* PROPERTY definition @modelCorrectnessProperties:2
prop_1702664941455164000 ==
autopilot_sometimes
----
=============================================================================
\* Modification History
\* Created Fri Dec 15 21:29:01 MSK 2023 by mark
