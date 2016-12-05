import React, {Component} from 'react'
import * as UI from '../../UIKit'
import {Creator, Grid } from '../../entry'


const PointCheck = (props) => (
	<div>{props.name}</div>
)

class Form extends Component {

	componentWillMount = () => {
		this.props.points.actions.fetch();
	}

	render = () => {
	    return (<UI.Box>
	      <form onSubmit={this.props.onSubmit}>
	          <UI.RFInput label="Name" name="name"/>
	          <UI.Row>
	          	<UI.Col6>
	          		<Grid {...this.props.points} cols={1} plain={true} component={PointCheck}/>
	          	</UI.Col6>
	          	<UI.Col6>
	          		<Grid {...this.props.points} cols={1} plain={true} component={PointCheck}/>
	          	</UI.Col6>
	          </UI.Row>
	          <UI.Button type="submit" disabled={this.props.pristine || this.props.submitting} label="Submit"/>
	          &nbsp;
	          <UI.Button type="button" color="white" onClick={this.props.onClose} label="Close"/>
	      </form>
	    </UI.Box>)
    }
}

export default Form;