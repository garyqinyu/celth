

class ToggleButton : public PegTextButton
{
    public:
        ToggleButton(PegRect &Size, PEGCHAR *pText, WORD wId = 0,
            WORD wStyle = AF_ENABLED);
        SIGNED Message(const PegMessage &Mesg);

    private:
};

