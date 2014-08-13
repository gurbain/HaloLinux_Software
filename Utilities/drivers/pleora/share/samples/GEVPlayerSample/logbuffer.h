// *****************************************************************************
//
//     Copyright (c) 2010, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <PvConfigurationReader.h>
#include <PvConfigurationWriter.h>

#include <QtCore/QMutex>
#include <QtCore/QString>
#include <QtCore/QTime>


class LogBuffer
{
public:

	LogBuffer();
	~LogBuffer();

	void Log( const QString &aItem );
	QString Empty();

	void Reset();

	void SetGenICamEnabled( bool aEnabled ) { mGenICamEnabled = aEnabled; }
	void SetEventsEnabled( bool aEnabled ) { mEventsEnabled = aEnabled; }
	void SetBufferAllEnabled( bool aEnabled ) { mBufferAllEnabled = aEnabled; }
	void SetBufferErrorEnabled( bool aEnabled ) { mBufferErrorEnabled = aEnabled; }
	void SetFilename( const QString &aFilename );
	void SetWriteToFileEnabled( bool aEnabled );

	bool IsGenICamEnabled() const { return mGenICamEnabled; }
	bool IsEventsEnabled() const { return mEventsEnabled; }
	bool IsBufferAllEnabled() const { return mBufferAllEnabled; }
	bool IsBufferErrorEnabled() const { return mBufferErrorEnabled; }
	const QString &GetFilename() const { return mFilename; }
	bool IsWriteToFileEnabled() const { return mWriteToFileEnabled; }

    // Persistence
    void Save( PvConfigurationWriter &aWriter );
    void Load( PvConfigurationReader &aReader );

    static QString Unpack( const char *aPattern, const unsigned char *aData, PvUInt32 aDataLength, int aGap );

protected:

	void ResetConfig();

private:

	QMutex mMutex;

	QTime mStartTimestamp;
	QString mBuffer;

	bool mGenICamEnabled;
	bool mEventsEnabled;
	bool mBufferAllEnabled;
	bool mBufferErrorEnabled;

	QString mFilename;
	bool mWriteToFileEnabled;
};

