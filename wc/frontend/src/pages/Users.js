import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'

class Users extends Component
{
	render = () => {
		return (<div>Users</div>);
	}
}

export default connect(state => { return state; })(Users)