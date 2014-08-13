/*
 * bufferoptions.cpp
 *
 *  Created on: Dec 3, 2009
 *      Author: fgobeil
 */

#include "gevplayer.h"
#include "bufferoptionsdlg.h"

#include <PvPipeline.h>

#include <QtGui/QHBoxLayout> 
#include <QtGui/QGroupBox>
#include <QtGui/QMessageBox>

#include <assert.h>


#define BUFFEROPTIONSDLG_VERSION ( "1.0.0.0" )

#define TAG_VERSION ( "bufferoptionsdlgversion" )
#define TAG_STREAMINGBUFFERCOUNT ( "streamingbuffercount" )
#define TAG_STREAMINGDEFAULTBUFFERSIZE ( "streamingdefaultbuffersize" )
#define TAG_AUTORESIZE ( "streamingautoresize" )

#define BUFFER_COUNT_DEFAULT ( 16 );
#define WARNING_MEM_THRESHOLD ( 1 * 1024 * 1024 * 1024 ) // 1 GB


BufferOptionsDlg::BufferOptionsDlg( PvPipeline *aPipeline, GEVPlayer* aGEVPlayerDlg )
    : QDialog( ( QWidget* ) aGEVPlayerDlg )
	, mPipeline( aPipeline )
	, mGEVPlayerDlg( aGEVPlayerDlg )
{
	CreateLayout();

	mBufferCount = mPipeline->GetBufferCount();
	mOriginalBufferCount = mBufferCount;

	QString lBufferCountString;
	lBufferCountString.setNum( mOriginalBufferCount );
	mBufferCountEdit->setText( lBufferCountString );

	mBufferSize = mPipeline->GetDefaultBufferSize();
	mOriginalBufferSize = mBufferSize;

	QString lBufferSizeString;
	lBufferSizeString.setNum( mOriginalBufferSize );
	mBufferSizeEdit->setText( lBufferSizeString );

	mAutoResize = mPipeline->GetHandleBufferTooSmall();
	mOriginalAutoResize = mAutoResize;
	mAutoResizeCheck->setChecked( mAutoResize );
}


BufferOptionsDlg::~BufferOptionsDlg()
{
}


void BufferOptionsDlg::CreateLayout()
{
	// Buffer count

	mBufferCountEdit = new QLineEdit;
	mBufferCountEdit->setMinimumWidth( 80 );

	QHBoxLayout *lBufferCountLayout = new QHBoxLayout;
	lBufferCountLayout->addWidget( mBufferCountEdit );
	lBufferCountLayout->addWidget( new QLabel( "buffers" ) );

	QVBoxLayout *lBufferCountVLayout = new QVBoxLayout;
	lBufferCountVLayout->addLayout( lBufferCountLayout );
	lBufferCountVLayout->addStretch();

	QLabel *lLabel = new QLabel( "Increasing the buffer count can make streaming more robust to missing block IDs, but at the expense of using more memory and increasing potential latency.\r\n\r\n"
		"Applications using slow frame rates or using very large buffers are not as sensitive to missing block IDs and can thus save memory by using only 4 or 8 buffers.\r\n\r\n"
		"Using more than 16 buffers is typically used in high frame rate, small buffer applications." );
	lLabel->setAlignment( Qt::AlignTop | Qt::AlignLeft );
	lLabel->setWordWrap( true );
	lLabel->setMinimumWidth( 400 );

	QHBoxLayout *lHLayout1 = new QHBoxLayout;
	lHLayout1->addLayout( lBufferCountVLayout );
	lHLayout1->addStretch();
	lHLayout1->addWidget( lLabel );

	QGroupBox *lBufferCountBox = new QGroupBox( "Buffers used for streaming" );
	lBufferCountBox->setLayout( lHLayout1 );

	// Default buffer size

	mBufferSizeEdit = new QLineEdit;
	mBufferSizeEdit->setMinimumWidth( 80 );

	QHBoxLayout *lBufferSizeLayout = new QHBoxLayout;
	lBufferSizeLayout->addWidget( mBufferSizeEdit );
	lBufferSizeLayout->addWidget( new QLabel( "bytes" ) );

	QVBoxLayout *lBufferSizeVLayout = new QVBoxLayout;
	lBufferSizeVLayout->addLayout( lBufferSizeLayout );
	lBufferSizeVLayout->addStretch();

	lLabel = new QLabel( "The default buffer size is used to allocate the acquisition pipeline buffers when it is not possible to directly read the payload size from the device.\r\n\r\n"
		"The default buffer size can be computed using this formula:\r\n"
		"(((width * pixel bytes) + padding x) * height) + padding y" );
	lLabel->setAlignment( Qt::AlignTop | Qt::AlignLeft );
	lLabel->setWordWrap( true );
	lLabel->setMinimumWidth( 400 );

	QHBoxLayout *lHLayout2 = new QHBoxLayout;
	lHLayout2->addLayout( lBufferSizeVLayout );
	lHLayout2->addStretch();
	lHLayout2->addWidget( lLabel );

	QGroupBox *lBufferSizeBox = new QGroupBox( "Default buffer size" );
	lBufferSizeBox->setLayout( lHLayout2 );

	// Automatic buffer resizing

	mAutoResizeCheck = new QCheckBox( "Enabled" );

	QVBoxLayout *lAutoResizeVLayout = new QVBoxLayout;
	lAutoResizeVLayout->addWidget( mAutoResizeCheck );
	lAutoResizeVLayout->addStretch();

	lLabel = new QLabel( "If enabled, buffers are automatically resized by the acquisition pipeline when buffers with BUFFER_TOO_SMALL operation result are encountered." );
	lLabel->setAlignment( Qt::AlignTop | Qt::AlignLeft );
	lLabel->setWordWrap( true );
	lLabel->setMinimumWidth( 400 );

	QHBoxLayout *lHLayout3 = new QHBoxLayout;
	lHLayout3->addLayout( lAutoResizeVLayout );
	lHLayout3->addStretch();
	lHLayout3->addWidget( lLabel );

	QGroupBox *lAutoResizeBox = new QGroupBox( "Automatic buffer resizing" );
	lAutoResizeBox->setLayout( lHLayout3 );

	// OK and cancel buttons

	QHBoxLayout *lButtons = new QHBoxLayout;
    mOKButton = new QPushButton( tr( "OK" ) );
    mCancelButton = new QPushButton( tr( "Cancel" ) );
    lButtons->addStretch();
    lButtons->addWidget( mOKButton );
    lButtons->addWidget( mCancelButton );

    QObject::connect( mOKButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    QObject::connect( mCancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // Take everything above and build the dialog layout

    QVBoxLayout *lMain = new QVBoxLayout;
    lMain->addWidget( lBufferCountBox );
    lMain->addWidget( lBufferSizeBox );
    lMain->addWidget( lAutoResizeBox );
    lMain->addStretch();
    lMain->addLayout( lButtons );

    // Configure dialog

    setLayout( lMain );
    setFixedSize( 700, 520 );

    Qt::WindowFlags lFlags = windowFlags();
    lFlags |= Qt::CustomizeWindowHint;
    lFlags &= ~Qt::WindowContextHelpButtonHint;
    lFlags &= ~Qt::WindowSystemMenuHint;

    lFlags |= Qt::WindowStaysOnTopHint;
    setWindowFlags( lFlags );

    setWindowTitle( tr( "Buffer Options" ) );
}


void BufferOptionsDlg::accept()
{
	// Get default buffer count string
	QString lBufferCountString = mBufferCountEdit->text();

	// Convert default buffer size to integer
	bool lOK = false;
	PvUInt32 lCount = lBufferCountString.toInt( &lOK );
	if ( !lOK )
	{
		// Parsing error
        QMessageBox::warning( this, "Error", "Invalid buffer count value." );
		return;
	}
	if ( lCount <= 0 )
	{
		// We allow just about anything, but not 0
        QMessageBox::warning( this, "Error", "Buffer count cannot be zero." );
		return;
	}

	// Get default buffer size string
	QString lBufferSizeString = mBufferSizeEdit->text();

	// Convert default buffer size to integer
	lOK = false;
	PvUInt32 lDefaultBufferSize = lBufferSizeString.toInt( &lOK );
	if ( !lOK )
	{
		// Parsing error
        QMessageBox::warning( this, "Error", "Invalid default buffer size value." );
		return;
	}
	if ( lDefaultBufferSize <= 0 )
	{
		// We allow just about anything, but not 0
        QMessageBox::warning( this, "Error", "Default buffer size cannot be zero." );
		return;
	}

	// Auto resize check box
	bool lAutoResize = mAutoResizeCheck->isChecked();

	if ( ( lCount == mOriginalBufferCount ) &&
		 ( lDefaultBufferSize == mOriginalBufferSize ) &&
		 ( lAutoResize == mOriginalAutoResize ) )
	{
		// Nothing changed, nothing to do...
		QDialog::accept();
		return;
	}

	// If *A LOT* of memory is about to be allocated, ask the user first
	PvUInt64 lTotal = lDefaultBufferSize * lCount;
	if ( lTotal >= WARNING_MEM_THRESHOLD )
	{
		double lGB = static_cast<double>( lTotal ) / static_cast<double>( 1024 * 1024 * 1024 );

		QString lMessage;
		lMessage.sprintf( "%.1f GB of memory will be required by the acquisition pipeline. Are you sure you want to apply these settings?", lGB );

		QMessageBox::StandardButtons lRetVal = QMessageBox::question( this, "Warning", lMessage,
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes );
		if ( lRetVal == QMessageBox::Cancel )
		{
			reject();
			return;
		}
		else if ( lRetVal != QMessageBox::Yes )
		{
			return;
		}
	}

	// Apply parameters
	mGEVPlayerDlg->StopStreaming();
	mPipeline->SetDefaultBufferSize( lDefaultBufferSize );
    PvResult lResult = mPipeline->SetBufferCount( lCount );
    mPipeline->SetHandleBufferTooSmall( lAutoResize );
	mGEVPlayerDlg->StartStreaming();
    if ( !lResult.IsOK() )
    {   
        QString lMessage;
        lMessage.sprintf( "%s\r\n\r\nCould not change buffer count to requested value.\r\nOnly %i buffers could be allocated.", 
            lResult.GetCodeString().GetAscii(), mPipeline->GetBufferCount() );

        QMessageBox::critical( this, "Error", lMessage );
    }

    QDialog::accept();
    return;
}


void BufferOptionsDlg::Save( PvConfigurationWriter &aWriter, PvPipeline *aPipeline )
{
    QString lStr;

    // Save a version string, just in case we need it in the future
    aWriter.Store( BUFFEROPTIONSDLG_VERSION, TAG_VERSION );

    // Buffer count
    aWriter.Store( lStr.setNum( aPipeline->GetBufferCount() ).toAscii().data(), TAG_STREAMINGBUFFERCOUNT );

    // Default buffer size
    aWriter.Store( lStr.setNum( aPipeline->GetDefaultBufferSize() ).toAscii().data(), TAG_STREAMINGDEFAULTBUFFERSIZE );

    // Auto resize
    aWriter.Store( aPipeline->GetHandleBufferTooSmall() ? "true" : "false", TAG_AUTORESIZE );
}


void BufferOptionsDlg::Load( PvConfigurationReader &aReader, PvPipeline *aPipeline )
{
    PvResult lResult;
    PvString lPvStr;

    int lBufferCount = aPipeline->GetBufferCount();
    lResult = aReader.Restore( TAG_STREAMINGBUFFERCOUNT, lPvStr );
    if ( lResult.IsOK() )
    {
        int lValue = QString( lPvStr.GetAscii() ).toInt();
        if ( lValue > 0 )
        {
        	lBufferCount = lValue;
        }
    }

    int lBufferSize = aPipeline->GetDefaultBufferSize();
    lResult = aReader.Restore( TAG_STREAMINGDEFAULTBUFFERSIZE, lPvStr );
    if ( lResult.IsOK() )
    {
    	int lValue = QString( lPvStr.GetAscii() ).toInt();
    	if ( lValue > 0 )
    	{
    		lBufferSize = lValue;
    	}
    }

    bool lAutoResize = true;
    lResult = aReader.Restore( TAG_AUTORESIZE, lPvStr );
    if ( lResult.IsOK() )
    {
        // Comparing to false as the default, usual behaviour is true
        lAutoResize = lPvStr != "false";
    }

    aPipeline->SetDefaultBufferSize(lBufferSize);
    aPipeline->SetBufferCount( lBufferCount );
    aPipeline->SetHandleBufferTooSmall( lAutoResize );
}



