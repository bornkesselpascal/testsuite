# Parameters

test_name      = "ihwk_C1"
test_duration  = "80"

test_dynamic   = True
test_dyn_mode  = "DATAGRAM_SIZE"     # DISABLED, DATAGRAM_SIZE, CYCLE_TIME
test_dyn_min   = "80"
test_dyn_max   = "8080"
test_dyn_steps = "8"

test_type      = "ST_PACKET"       # ST_UDP, ST_RAW, ST_PACKET
test_cycletime = "1"
test_datagrams = ["80"]

test_latency   = "END_TO_END"   # DISABLED, END_TO_END, FULL
