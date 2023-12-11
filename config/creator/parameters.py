# Parameters

test_name      = "ihwk_A2"
test_duration  = "100"

test_dynamic   = True
test_dyn_mode  = "DATAGRAM_SIZE"     # DISABLED, DATAGRAM_SIZE, CYCLE_TIME
test_dyn_min   = "9080"
test_dyn_max   = "64080"
test_dyn_steps = "10"

test_type      = "ST_UDP"       # ST_UDP, ST_RAW, ST_PACKET
test_cycletime = "1"
test_datagrams = ["80"]

test_latency   = "END_TO_END"   # DISABLED, END_TO_END, FULL
