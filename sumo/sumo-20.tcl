
# Auto-generated Tcl script to run SUMO simulation
set net "grid.net.xml"
set route "sumo-20.rou.xml"
set cfg "simulation.sumocfg"

exec sumo -n $net -r $route --step-length 1
