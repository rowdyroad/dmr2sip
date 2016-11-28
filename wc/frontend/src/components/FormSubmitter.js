import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'
import Request from '../utils/Request';





class FormSubmitter extends Component
{
	onSubmit = (value) => {
		if (this.props.validate) {
			if (!this.props.validate()) {
				return false;
			}
		}
		Request.post(this.props.url, {

		})
	}

	render = () => {

	}
}

export default FormSubmitter;