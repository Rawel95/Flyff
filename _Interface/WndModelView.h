#ifdef __ZCK_VISU_MODELS
class FLItemElem;

class CWndModelView : public CWndNeuz
{
private:
	CMover m_Mover;
	CModelObject* m_pPet;
	CModelObject* m_pEgg;
	FLOAT m_fRot;
	FLOAT m_fWheel;
	CPoint m_ptOld;
	BOOL m_bLButtonDownRot;

	BOOL SetSex( BOOL bSex );
	void UpdateMover();
	void UpdateItem( DWORD dwParts, FLItemElem& itemElem );
	void RenderPreviewModel( C2DRender* p2DRender, CModelObject* pModel, CMover* pMover );

public:
	CWndModelView();
	virtual ~CWndModelView();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwType = 0 );
	virtual void OnInitialUpdate();
	virtual BOOL Process();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnMouseMove( UINT nFlags, CPoint point );
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );

	BOOL SetItem( DWORD dwItemID );
	BOOL SetItem( FLItemElem* pItemElem );
	static BOOL CanPreview( PT_ITEM_SPEC pItemSpec );
};
#endif // __ZCK_VISU_MODELS