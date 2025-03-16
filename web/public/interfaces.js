export var Status;
(function (Status) {
    Status[Status["not_started"] = 0] = "not_started";
    Status[Status["ready"] = 1] = "ready";
    Status[Status["running"] = 2] = "running";
    Status[Status["finished"] = 3] = "finished";
})(Status || (Status = {}));
