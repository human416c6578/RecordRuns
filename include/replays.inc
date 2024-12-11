enum eHeader{
	hTimestamp,
	hVersion,
	hTime,
	hMap[64],
	hName[64],
	hSteamID[32],
	hInfo[32]
}

enum eFrame{
	fTimestamp,
	Float:fOrigin[3],
	Float:fAngles[2],
	fSpeed,
	fFps,
	fKeys,
	fGrounded,
	fGravity
}

native LoadReplay(id, path[], header[eHeader]);
native SaveReplay(path[], id, map[], authid[], category[], time);
native StartRecord(id);
native StopRecord(id);
native GetNextFrame(frame[eFrame]);
native GetCurrentReplay();
native SetCurrentReplay(id);
native NextReplay();
native SkipFrames(frames);
native DeleteReplay(recordId);
native GetReplaySize();

//TODO
native GetReplayFrameTime(); // temporarly