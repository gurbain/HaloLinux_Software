// *****************************************************************************
//
//     Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __ACTIONCOMMANDDLG_H__
#define __ACTIONCOMMANDDLG_H__

#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QTextEdit>
#include <QtGui/QListWidget>
#include <QtGui/QLabel>

#include <PvActionCommand.h>


class GEVPlayer;


class ActionCommandDlg : public QDialog
{
    Q_OBJECT

public:

    ActionCommandDlg( GEVPlayer *aGEVPlayer );
	virtual ~ActionCommandDlg();

protected:

	void FillNetworkList();
	void CreateLayout();
	void EnableInterface();

	bool Configure();
	bool Send();
	void MonitorAcknowledges();

	bool EditToInt( QLineEdit *aEdit, PvUInt64 aMax, PvUInt64 &aValue );

protected slots:

    void OnSendClick();
    void OnScheduledStateChanged( int aState );

private:

	QListWidget *mNetworkList;
	QLineEdit *mDeviceKeyEdit;
	QLineEdit *mGroupKeyEdit;
	QLineEdit *mGroupMaskEdit;
	QCheckBox *mScheduledCheck;
	QLabel *mTimestampLabel;
	QLineEdit *mTimestampEdit;
	QCheckBox *mForceACKFlagCheck;
	QPushButton *mSendButton;
	QTextEdit *mAcknowledgesEdit;

	PvActionCommand mActionCommand;
	bool mForceACKFlag;
};


#endif /* __ACTIONCOMMANDDLG_H__ */


