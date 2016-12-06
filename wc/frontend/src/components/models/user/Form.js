import React from 'react'
import * as UI from '../../UIKit'

export default (props) => (
    <UI.Box>
      <form onSubmit={props.onSubmit}>
      	<UI.Row>
      		<UI.Col2 >
      			<div>Username</div>
      			<h3>{props.data && props.data.username}</h3>
      			{props.data && props.data.is_admin ? <div>Full Access</div> : <div>Restricted Access</div>}
      		</UI.Col2>
      		<UI.Col10>
	      	  <UI.RFInput label="Name" name="name"/>
	          <UI.RFInput label="Password" name="password"  type="password"/>
	          <UI.Button type="submit" disabled={props.pristine || props.submitting} label="Submit"/>
	          &nbsp;
	          <UI.Button type="button" color="white" onClick={props.onClose} label="Close"/>
	        </UI.Col10>
	    </UI.Row>
      </form>
    </UI.Box>
)