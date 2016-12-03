import React from 'react'
import * as UI from '../UIKit'

export default (props) => (
	<UI.Box style={{textAlign:'center'}}>
		<h1 style={{padding:'1em'}}>Do you really want to remove current element <b>{props.data.name}</b>?</h1>
	    <UI.Button label="Remove" color="danger" onClick={()=>{ props.actions.delete(props.data)}}/>
	    &nbsp;
	    <UI.Button label="Cancel" color="white" onClick={()=> { props.actions.close(props.data)}}/>
	</UI.Box>
)
