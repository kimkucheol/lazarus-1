{%MainUnit cocoaint.pas}

{******************************************************************************
  All Cocoa interface communication implementations.
  This is the implementation of the overrides of the Cocoa Interface for the
  methods defined in the
  lcl/include/lclintf.inc


  !! Keep alphabetical !!

 ******************************************************************************
 Implementation
 ******************************************************************************

 *****************************************************************************
  This file is part of the Lazarus Component Library (LCL)

  See the file COPYING.modifiedLGPL.txt, included in this distribution,
  for details about the license.
 *****************************************************************************
}

{------------------------------------------------------------------------------
  Method:  CreateStandardCursor
  Params:  ACursor - Cursor type
  Returns: Cursor object in Cocoa for the specified cursor type
 ------------------------------------------------------------------------------}
function TCocoaWidgetSet.CreateStandardCursor(ACursor: SmallInt): HCursor;
begin
  case ACursor of
    crArrow, crDefault: Result := HCursor(TCocoaCursor.CreateStandard(NSCursor.arrowCursor));
    crCross       : Result := HCursor(TCocoaCursor.CreateStandard(NSCursor.crosshairCursor));
    crIBeam       : Result := HCursor(TCocoaCursor.CreateStandard(NSCursor.IBeamCursor));
    crSizeNS,
    crVSplit      : Result := HCursor(TCocoaCursor.CreateStandard(NSCursor.resizeUpDownCursor));
    crSizeNESW,
    crSizeNWSE,
    crSizeWE,
    crHSplit      : Result := HCursor(TCocoaCursor.CreateStandard(NSCursor.resizeLeftRightCursor));
    crSizeN       : Result := HCursor(TCocoaCursor.CreateStandard(NSCursor.resizeUpCursor));
    crSizeNW,
    crSizeSW,
    crSizeW       : Result := HCursor(TCocoaCursor.CreateStandard(NSCursor.resizeLeftCursor));
    crSizeNE,
    crSizeSE,
    crSizeE       : Result := HCursor(TCocoaCursor.CreateStandard(NSCursor.resizeRightCursor));
    crSizeS       : Result := HCursor(TCocoaCursor.CreateStandard(NSCursor.resizeDownCursor));
    crNo,
    crNoDrop      : Result := HCursor(TCocoaCursor.CreateStandard(NSCursor.operationNotAllowedCursor));
    crHandPoint   : Result := HCursor(TCocoaCursor.CreateStandard(NSCursor.pointingHandCursor));
    //crHourGlass,
    crDrag        : Result := HCursor(TCocoaCursor.CreateStandard(NSCursor.dragCopyCursor));
  else
    // We answer with Result=0 for crHourGlass because Cocoa does not provide any API
    // to set the wait cursor. As a compromise to make cross-platform LCL apps written
    // in Windows/Linux behave as expected without change, we answer 0 here and
    // a non-native wait cursor will be utilized
    Result := 0;
  end;
end;
(*
{------------------------------------------------------------------------------
  Method:  DrawGrid
  Params:  DC     - Handle to device context
           R      - Grid rectangle
           DX, DY - Grid cell width and height

  Draws the point grid
 ------------------------------------------------------------------------------}
procedure TCarbonWidgetSet.DrawGrid(DC: HDC; const R: TRect; DX, DY: Integer);
begin
  {$IFDEF VerboseLCLIntf}
    DebugLn('TCarbonWidgetSet.DrawGrid Rect: ' + DbgS(R));
  {$ENDIF}
  
  if not CheckDC(DC, 'DrawGrid') then Exit;

  TCarbonDeviceContext(DC).DrawGrid(R, DX, DY);
end;

function TCarbonWidgetSet.ExtUTF8Out(DC: HDC; X, Y: Integer; Options: Longint;
  Rect: PRect; Str: PChar; Count: Longint; Dx: PInteger): Boolean;
begin
  Result:=inherited ExtUTF8Out(DC, X, Y, Options, Rect, Str, Count, Dx);
end;

function TCarbonWidgetSet.FontCanUTF8(Font: HFont): boolean;
begin
  Result := True;
end;

function TCarbonWidgetSet.GetAcceleratorString(const AVKey: Byte;
  const AShiftState: TShiftState): String;
begin
  Result:=inherited GetAcceleratorString(AVKey, AShiftState);
end;

function TCarbonWidgetSet.GetControlConstraints(Constraints: TObject): boolean;
begin
  Result:=inherited GetControlConstraints(Constraints);
end;

{------------------------------------------------------------------------------
  Method:  GetDesignerDC
  Params:  WindowHandle - Handle of window
  Returns: Device context for window designer
 ------------------------------------------------------------------------------}
function TCarbonWidgetSet.GetDesignerDC(WindowHandle: HWND): HDC;
begin
  Result := 0;
  {$IFDEF VerboseLCLIntf}
    DebugLn('TCarbonWidgetSet.GetDesignerDC Handle: ' + DbgS(WindowHandle));
  {$ENDIF}

  if not CheckWidget(WindowHandle, 'GetDesignerDC', TCarbonDesignWindow) then Exit;
  Result := HDC(TCarbonDesignWindow(WindowHandle).GetDesignContext);
end;

{------------------------------------------------------------------------------
  Method:  GetLCLOwnerObject
  Params:  Handle - Handle of window
  Returns: LCL control which has the specified widget
 ------------------------------------------------------------------------------}
function TCarbonWidgetSet.GetLCLOwnerObject(Handle: HWnd): TObject;
begin
  {$IFDEF VerboseLCLIntf}
    DebugLn('TCarbonWidgetSet.GetLCLOwnerObject Handle: ' + DbgS(Handle));
  {$ENDIF}
  
  Result := nil;
  if not CheckWidget(Handle, 'GetLCLOwnerObject') then Exit;
  
  Result := TCarbonWidget(Handle).LCLObject;
  
  {$IFDEF VerboseLCLIntf}
    DebugLn('TCarbonWidgetSet.GetLCLOwnerObject Result: ' + DbgS(Result));
  {$ENDIF}
end;
*)
{------------------------------------------------------------------------------
  Method:  IsDesignerDC
  Params:  WindowHandle - Handle of window
           DC           - Handle of device context
  Returns: If the device context is designer
 ------------------------------------------------------------------------------}
function TCocoaWidgetSet.IsDesignerDC(WindowHandle: HWND; DC: HDC): Boolean;
begin
  Result := (WindowHandle <> 0) and (DC <> 0) and TCocoaContext(DC).isDesignDC;
end;

procedure TCocoaWidgetSet.InitClipboard;
begin
  PrimarySelection := NSPasteboard.pasteboardWithUniqueName();
  SecondarySelection := NSPasteboard.pasteboardWithUniqueName();
  ClipboardFormats := TFPObjectList.Create(True);
end;

procedure TCocoaWidgetSet.FreeClipboard;
begin
  PrimarySelection.releaseGlobally();
  SecondarySelection.releaseGlobally();
  ClipboardFormats.Free;
end;

procedure TCocoaWidgetSet.SyncClipboard();
var
  ct : TClipboardType;
  pb : NSPasteboard;
begin
  for ct := low(TClipboardType) to high(TClipboardType) do begin
    if not Pasteboards[ct].isOwned then Continue;

    pb := Pasteboards[ct].pasteboard;
    if not Assigned(pb) then Continue;

    if (pb.changeCount <> Pasteboards[ct].changeCount) then
    begin
      Pasteboards[ct].isOwned:=false;
      if Assigned(Pasteboards[ct].dataProc) then
        // notifying about the loss of ownership
        Pasteboards[ct].dataProc(0, nil);
    end;
  end;
end;

function TCocoaWidgetSet.GetClipboardDataForFormat(AFormat: TClipboardFormat): TCocoaClipboardData;
begin
  Result := TCocoaClipboardData(AFormat);
end;

{------------------------------------------------------------------------------
  Func:    CocoaPromptUser
  Params:  DialogCaption - Dialog caption
           DialogMessage - Dialog message text
           DialogType    - Type of dialog
           Buttons       - Pointer to button types
           ButtonCount   - Count of passed buttons
           DefaultIndex  - Index of default button
           EscapeResult  - Result value of escape
           sheetOfWindow - Shows the prompt as a sheet to the specified NSWindow,
                           if nil, the prompt is shown as an application modal dialog
  Returns: The result value of pushed button

  Shows modal dialog or window sheet with the specified caption, message
  and buttons and prompts user to push one.
 ------------------------------------------------------------------------------}
function CocoaPromptUser(const DialogCaption : string;
                                     const DialogMessage : string;
                                           DialogType    : LongInt;
                                           Buttons       : PLongInt;
                                           ButtonCount   : LongInt;
                                           DefaultIndex  : LongInt;
                                           EscapeResult  : LongInt;
                                           sheetOfWindow : NSWindow) : LongInt;
{Implements MessageDlg.}

const
  ButtonCaption: Array [idButtonOk..idButtonNoToAll] of PChar =
    ('OK', 'Cancel', ''{Help}, 'Yes', 'No', 'Close', 'Abort', 'Retry', 'Ignore',
     'All', 'Yes To All', 'No To All');
var
  anAlert: NSAlert;
  informativeText: NSString;
  messageText: NSString;
  I: Integer;
  aButton: NSButton;
  Str: string;
begin
  {Str := 'TCocoaWidgetSet.PromptUser DialogCaption: ' + DialogCaption +
    ' DialogMessage: ' + DialogMessage + ' DialogType: ' + DbgS(DialogType) +
    ' ButtonCount: ' + DbgS(ButtonCount) + ' DefaultIndex: ' +
    DbgS(DefaultIndex) + ' EscapeResult: ' + DbgS(EscapeResult);
  Result := -1;}
  {$IFDEF VerboseLCLIntf}
    DebugLn('TCocoaWidgetSet.PromptUser DialogCaption: ' + DialogCaption +
      ' DialogMessage: ' + DialogMessage + ' DialogType: ' + DbgS(DialogType) +
      ' ButtonCount: ' + DbgS(ButtonCount) + ' DefaultIndex: ' +
      DbgS(DefaultIndex) + ' EscapeResult: ' + DbgS(EscapeResult));
  {$ENDIF}
  Result := -1;
  AnAlert := NSAlert.alloc.init;
  try
    informativeText := NSStringUtf8(DialogMessage);
    messageText := NSStringUtf8(DialogCaption);
    case DialogType of
      idDialogWarning,
      idDialogError    : anAlert.setAlertStyle(NSCriticalAlertStyle);
      idDialogInfo     : anAlert.setAlertStyle(NSInformationalAlertStyle);
    end;

    try
      anAlert.setInformativeText(informativeText);
      anAlert.setMessageText(messageText);

      for I := 0 to ButtonCount - 1 do
      begin
        if Buttons[I] = idButtonHelp then
        begin
          anAlert.setShowsHelp(true)
          {$IFDEF VerboseLCLIntf}
          DebugLn('TCocoaWidgetSet.PromptUser Warning: Help button is shown but ignored');
          {$ENDIF}
        end
        else
        begin
          if (Buttons[I] < Low(ButtonCaption)) or (Buttons[I] > High(ButtonCaption)) then
          begin
            DebugLn('TCocoaWidgetSet.PromptUser Invalid button ID: ' + DbgS(Buttons[I]));
            Continue;
          end;

          aButton := anAlert.addButtonWithTitle(NSLocalizedString(NSSTR(ButtonCaption[Buttons[I]])));
          aButton.setKeyEquivalentModifierMask(0);

          if I = DefaultIndex then
            aButton.setKeyEquivalent(NSSTR(#13))
          else if I = 0 then
            // By default, the first button is the default button. If in our
            // case this should not be the case, remove the default status
            // from the first button.
            aButton.setKeyEquivalent(NSSTR(''));

          if Buttons[I]=mrCancel then
            aButton.setKeyEquivalent(NSSTR(#27));

          aButton.setTag(Buttons[I]);
        end;
      end;

      if Assigned(sheetOfWindow) then
      begin
        anAlert.beginSheetModalForWindow_modalDelegate_didEndSelector_contextInfo(
          sheetOfWindow, nil, nil, nil
        );
        Result := 0;
      end
      else
        Result := AnAlert.runModal;
    finally
      informativeText.release;
      messageText.release;
    end;

  finally
    AnAlert.release;
  end;

  {$IFDEF VerboseLCLIntf}
    DebugLn('TCocoaWidgetSet.PromptUser Result: ' + DbgS(Result));
  {$ENDIF}
end;  {TCocoaWidgetSet.PromptUser}

{------------------------------------------------------------------------------
  Method:  PromptUser
  Params:  DialogCaption - Dialog caption
           DialogMessage - Dialog message text
           DialogType    - Type of dialog
           Buttons       - Pointer to button types
           ButtonCount   - Count of passed buttons
           DefaultIndex  - Index of default button
           EscapeResult  - Result value of escape
  Returns: The result value of pushed button

  Shows modal dialog with the specified caption, message and buttons and prompts
  user to push one.
 ------------------------------------------------------------------------------}
function TCocoaWidgetSet.PromptUser(const DialogCaption : string;
                                     const DialogMessage : string;
                                           DialogType    : LongInt;
                                           Buttons       : PLongInt;
                                           ButtonCount   : LongInt;
                                           DefaultIndex  : LongInt;
                                           EscapeResult  : LongInt) : LongInt;
begin
  Result := CocoaPromptUser(DialogCaption, DialogMessage, DialogType, Buttons, ButtonCount,
    DefaultIndex, EscapeResult);
end;  {TCocoaWidgetSet.PromptUser}

{------------------------------------------------------------------------------
  Function: RawImage_CreateBitmaps
  Params: ARawImage: Source raw image
          ABitmap:   Destination bitmap object
          AMask:     Destination mask object
          ASkipMask: When set, no mask is created
  Returns:

 ------------------------------------------------------------------------------}
function TCocoaWidgetSet.RawImage_CreateBitmaps(const ARawImage: TRawImage; out ABitmap, AMask: HBitmap; ASkipMask: Boolean): Boolean;
const
  ALIGNMAP: array[TRawImageLineEnd] of TCocoaBitmapAlignment = (cbaByte, cbaByte, cbaWord, cbaDWord, cbaQWord, cbaDQWord);
var
  ADesc: TRawImageDescription absolute ARawImage.Description;
  bmpType: TCocoaBitmapType;
begin
  Result := RawImage_DescriptionToBitmapType(ADesc, bmpType);
  if not Result then begin
    debugln(['TCocoaWidgetSet.RawImage_CreateBitmaps TODO Depth=',ADesc.Depth,' alphaprec=',ADesc.AlphaPrec,' byteorder=',ord(ADesc.ByteOrder),' alpha=',ADesc.AlphaShift,' red=',ADesc.RedShift,' green=',adesc.GreenShift,' blue=',adesc.BlueShift]);
    exit;
  end;
  ABitmap := HBITMAP(TCocoaBitmap.Create(ADesc.Width, ADesc.Height, ADesc.Depth, ADesc.BitsPerPixel, ALIGNMAP[ADesc.LineEnd], bmpType, ARawImage.Data));

  if ASkipMask or (ADesc.MaskBitsPerPixel = 0)
  then AMask := 0
  else AMask := HBITMAP(TCocoaBitmap.Create(ADesc.Width, ADesc.Height, 1, ADesc.MaskBitsPerPixel, ALIGNMAP[ADesc.MaskLineEnd], cbtMask, ARawImage.Mask));

  Result := True;
end;

{------------------------------------------------------------------------------
  Function: RawImage_DescriptionFromBitmap
  Params: ABitmap:
          ADesc:
  Returns:

  Describes the inner format utilized by Cocoa and specific information
  for the specified bitmap
 ------------------------------------------------------------------------------}
function TCocoaWidgetSet.RawImage_DescriptionFromBitmap(ABitmap: HBITMAP; out ADesc: TRawImageDescription): Boolean;
begin
  if CheckBitmap(ABitmap, 'RawImage_DescriptionFromBitmap')
  then Result := RawImage_DescriptionFromCocoaBitmap(ADesc, TCocoaBitmap(ABitmap))
  else Result := False;
end;

{------------------------------------------------------------------------------
  Function: RawImage_DescriptionFromDevice
  Params: ADC:   - Handle to device context
          ADesc: - Pointer to raw image description
  Returns: True if success

  Retrieves the standard image format utilized by Cocoa
 ------------------------------------------------------------------------------}
function TCocoaWidgetSet.RawImage_DescriptionFromDevice(ADC: HDC; out ADesc: TRawImageDescription): Boolean;
begin
  Result := False;

  FillStandardDescription(ADesc);
  if (ADC <> 0) and CheckDC(ADC, 'RawImage_DescriptionFromDevice') then
  begin
    with TCocoaContext(ADC).Size do
    begin
      ADesc.Width := cx;
      ADesc.Height := cy;
    end;
  end;

  Result := True;
end;

{------------------------------------------------------------------------------
  Function: RawImage_FromBitmap
  Params: ARawImage: Image to create
          ABitmap:   Source bitmap
          AMask:     Source mask
          ARect:     Source rect (TODO)
  Returns: True if the function succeeds

  Creates a raw image from the specified bitmap
 ------------------------------------------------------------------------------}
function TCocoaWidgetSet.RawImage_FromBitmap(out ARawImage: TRawImage; ABitmap, AMask: HBITMAP; ARect: PRect = nil): Boolean;
begin
  if CheckBitmap(ABitmap, 'RawImage_FromBitmap')
  and ((AMask = 0) or CheckBitmap(AMask, 'RawImage_FromBitmap (mask)'))
  then Result := RawImage_FromCocoaBitmap(ARawImage, TCocoaBitmap(ABitmap), TCocoaBitmap(AMask), ARect)
  else Result := False;
end;

{------------------------------------------------------------------------------
  Method:  TCocoaWidgetSet.GetImagePixelData

  Used by RawImage_FromDevice. Copies the data from a CGImageRef into a local
  buffer.

  The buffer is created using GetMem, and the caller is responsible for using
  FreeMem to free the returned pointer.

  This function throws exceptions in case of errors and may return a nil pointer.
 ------------------------------------------------------------------------------}
function TCocoaWidgetSet.GetImagePixelData(AImage: CGImageRef; out bitmapByteCount: PtrUInt): Pointer;
var
  bitmapData: Pointer;
  context: CGContextRef = nil;
  colorSpace: CGColorSpaceRef;
  bitmapBytesPerRow, pixelsWide, pixelsHigh: PtrUInt;
  imageRect: CGRect;
begin
  Result := nil;

   // Get image width, height. The entire image is used.
  pixelsWide := CGImageGetWidth(AImage);
  pixelsHigh := CGImageGetHeight(AImage);
  imageRect.origin.x := 0.0;
  imageRect.origin.y := 0.0;
  imageRect.size.width := pixelsWide;
  imageRect.size.height := pixelsHigh;

  // The target format is fixed in ARGB, DQWord alignment, with 32-bits depth and
  // 8-bits per channel, the default image format on the LCL
  bitmapBytesPerRow   := ((pixelsWide * 4) + $F) and not PtrUInt($F);
  bitmapByteCount     := (bitmapBytesPerRow * pixelsHigh);

  // Use the generic RGB color space.
  colorSpace := CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
  if (colorSpace = nil) then
    raise Exception.Create('Unable to create CGColorSpaceRef');

  // Allocate memory for image data. This is the destination in memory
  // where any drawing to the bitmap context will be rendered.
  bitmapData := System.GetMem( bitmapByteCount );
  if (bitmapData = nil) then
    raise Exception.Create('Unable to allocate memory');

  { Creates the bitmap context.

    Regardless of what the source image format is, it will be converted
    over to the format specified here by CGBitmapContextCreate. }
  context := CGBitmapContextCreate(bitmapData,
                                   pixelsWide,
                                   pixelsHigh,
                                   8,      // bits per component
                                   bitmapBytesPerRow,
                                   colorSpace,
                                   kCGImageAlphaNoneSkipFirst); // The function fails with kCGImageAlphaFirst
  if (context = nil) then
  begin
    System.FreeMem(bitmapData);
    raise Exception.Create('Unable to create CGContextRef');
  end;

  // Draw the image to the bitmap context. Once we draw, the memory
  // allocated for the context for rendering will then contain the
  // raw image data in the specified color space.
  CGContextDrawImage(context, imageRect, AImage);

  // Now we can get a pointer to the image data associated with the context.
  // ToDo: Verify if we should copy this data to a new buffer
  Result := CGBitmapContextGetData(context);

  { Clean-up }
  CGColorSpaceRelease(colorSpace);
  CGContextRelease(context);
end;


{------------------------------------------------------------------------------
  Function: RawImage_FromDevice
  Params: ARawImage: Image to create
          ADC:       Source dc
          ARect:     Source rect (TODO)

  This function is utilized when the function TBitmap.LoadFromDevice is called

  The main use for this function is to get a screenshot.

  MWE: exept for the desktop, there is always a bitmep selected in the DC.
       So get this internal bitmap and pass it to RawImage_FromBitmap
       
 The ScreenShot getting code uses OpenGL to get a CGImageRef.
 
 The only way to access the bytes of a CGImageRef is by drawing it to a canvas
 and then reading the data from the canvas. In doing it we can choose the pixel
 format for the canvas, so we choose a convenient one: ARGB, 32-bits depth,
 just like the standard image description.

 See also: Technical Q&A QA1509 - Getting the pixel data from a CGImage object

 http://developer.apple.com/qa/qa2007/qa1509.html
 ------------------------------------------------------------------------------}
function TCocoaWidgetSet.RawImage_FromDevice(out ARawImage: TRawImage; ADC: HDC; const ARect: TRect): Boolean;
var
  CBC: TCocoaBitmapContext absolute ADC;
  Desc: TRawImageDescription absolute ARawImage.Description;
  displayID: CGDirectDisplayID;
  ScreenImage: CGImageRef;
begin
  Result := False;

  // Verifies if we are getting the rawimage from a normal DC as opposed to a
  // desktop DC
  if CheckDC(ADC, 'RawImage_FromDevice') and (CBC is TCocoaBitmapContext) then
  begin
    Result := RawImage_FromCocoaBitmap(ARawImage, CBC.Bitmap, nil, @ARect);
    Exit;
  end;

  { Screenshot taking code starts here }

  { Get's a screenshot }
  displayID := CGMainDisplayID();
  {$IF not defined(MAC_OS_X_VERSION_MIN_REQUIRED) or (MAC_OS_X_VERSION_MIN_REQUIRED >= 1060)}
  ScreenImage := CGDisplayCreateImage(displayID);
  {$ENDIF}

  { Fills the image description }
  ARawImage.Init;
  FillStandardDescription(ARawImage.Description);
  ARawImage.Description.Height := CGImageGetHeight(ScreenImage);
  ARawImage.Description.Width := CGImageGetWidth(ScreenImage);
  ARawImage.Mask := nil;

  { Copies the image data to a local buffer }
  ARawImage.Data := GetImagePixelData(ScreenImage, ARawImage.DataSize);

  { clean-up }
  CGImageRelease(ScreenImage);

  Result := True;
end;

{------------------------------------------------------------------------------
  Function: RawImage_QueryDescription
  Params: AFlags:
          ADesc:
  Returns:

 ------------------------------------------------------------------------------}
//function TCarbonWidgetSet.RawImage_QueryDescription(AFlags: TRawImageQueryFlags; var ADesc: TRawImageDescription): Boolean;
//begin
//  // override only when queried formats are different from screen description
//end;
(*
{------------------------------------------------------------------------------
  Method:  ReleaseDesignerDC
  Params:  Window - handle of window
           DC     - handle of designer device context
  Returns: 1 if the device context was released or 0 if it wasn't

  Releases a designer device context (DC)
 ------------------------------------------------------------------------------}
function TCarbonWidgetSet.ReleaseDesignerDC(Window: HWND; DC: HDC): Integer;
begin
  Result := 0;
  {$IFDEF VerboseLCLIntf}
    DebugLn('TCarbonWidgetSet.ReleaseDesignerDC Handle: ' + DbgS(Window));
  {$ENDIF}

  if not CheckWidget(Window, 'ReleaseDesignerDC', TCarbonDesignWindow) then Exit;
  TCarbonDesignWindow(Window).ReleaseDesignContext;
  Result := 1;
end;

{------------------------------------------------------------------------------
  Method:  SetMainMenuEnabled
  Params:  AEnabled

  Enables/disables main menu
 ------------------------------------------------------------------------------}
procedure TCarbonWidgetSet.SetMainMenuEnabled(AEnabled: Boolean);
begin
  {$IFDEF VerboseLCLIntf}
    DebugLn('TCarbonWidgetSet.SetMainMenuEnabled AEnabled: ' + DbgS(AEnabled));
  {$ENDIF}

  fMenuEnabled:=AEnabled;
  if FMainMenu <> 0 then
  begin
    if csDesigning in TCarbonMenu(FMainMenu).LCLMenuItem.ComponentState then Exit;
    TCarbonMenu(FMainMenu).SetEnable(AEnabled);
  end;
end;

{------------------------------------------------------------------------------
  Method:  TCarbonWidgetSet.SetRootMenu
  Params:  AMenu - Main menu

  Sets the menu to menu bar
 ------------------------------------------------------------------------------}
procedure TCarbonWidgetSet.SetRootMenu(const AMenu: HMENU);
begin
  {$IFDEF VerboseLCLIntf}
    DebugLn('TCarbonWidgetSet.SetRootMenu AMenu: ' + DbgS(AMenu));
  {$ENDIF}
  if (AMenu <> 0) and CheckMenu(AMenu, 'SetRootMenu') and 
     not (csDesigning in TCarbonMenu(AMenu).LCLMenuItem.ComponentState) then    
  begin
    TCarbonMenu(AMenu).AttachToMenuBar;    
    FMainMenu := AMenu;
    SetMainMenuEnabled(MenuEnabled);
  end;
end;

{------------------------------------------------------------------------------
  Method:  SetCaptureWidget
  Params:  AWidget - Carbon widget to capture

  Sets captured Carbon widget
 ------------------------------------------------------------------------------}
procedure TCarbonWidgetSet.SetCaptureWidget(const AWidget: HWND);
begin
  {$IFDEF VerboseLCLIntf}
    DebugLn('TCarbonWidgetSet.SetCaptureWidget AWidget: ' + DbgS(AWidget));
  {$ENDIF}
  
  if AWidget <> FCaptureWidget then
  begin
    FCaptureWidget := AWidget;
    
    if FCaptureWidget <> 0 then
      LCLSendCaptureChangedMsg(TCarbonWidget(FCaptureWidget).LCLObject);
  end;
end;

{------------------------------------------------------------------------------
  Method:  SetTextFractional
  Params:  ACanvas - LCL Canvas

  Sets canvas text fractional enabled
 ------------------------------------------------------------------------------}
procedure TCarbonWidgetSet.SetTextFractional(ACanvas: TCanvas; AEnabled: Boolean);
begin
   {$IFDEF VerboseLCLIntf}
    DebugLn('TCarbonWidgetSet.SetTextFractional ACanvas: ' + DbgS(ACanvas) + ' AEnabled: ' + DbgS(AEnabled));
  {$ENDIF}
  
  if not CheckDC(ACanvas.Handle, 'SetTextFractional') then Exit;
  
  TCarbonDeviceContext(ACanvas.Handle).TextFractional := AEnabled;
end;
*)

