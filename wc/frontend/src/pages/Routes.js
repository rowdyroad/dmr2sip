import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'

class Routes extends Component
{
	render = () => {
		return (<div>Routes</div>);
	}
}

export default connect(state => { return state; })(Routes)