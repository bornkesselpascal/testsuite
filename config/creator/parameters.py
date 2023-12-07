# Parameters

test_name      = "ihwk_7B"
test_duration  = "90"

test_dynamic   = True
test_dyn_mode  = "CYCLE_TIME"     # DISABLED, DATAGRAM_SIZE, CYCLE_TIME
test_dyn_min   = "1"
test_dyn_max   = "301"
test_dyn_steps = "15"

test_type      = "ST_UDP"       # ST_UDP, ST_RAW, ST_PACKET
test_cycletime = "1"
test_datagrams = ["65000"]

test_latency   = "END_TO_END"   # DISABLED, END_TO_END, FULL
