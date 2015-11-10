

class DialWindow : public PegDecoratedWindow
{
    public:
        DialWindow(const PegRect& Rect);
        ~DialWindow();

        SIGNED Message(const PegMessage& Mesg);

    private:
        PegDial* mpDial1;
        PegFiniteBitmapDial* mpDial2;
        PegFiniteBitmapDial* mpDial3;

        SIGNED miDial1Val;
        SIGNED miDial2Val;
        SIGNED miDial1Dir;
        SIGNED miDial2Dir;
};
