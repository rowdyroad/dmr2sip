import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'
import Request from '../utils/Request';


class ItemLoader extends Component
{
	state = {
		item:null
	}

	componentWillMount = () => {
		Request.get(this.props.url).then((data) => {
			this.setState({item: data});
		})
	}

	render = () => {
		if (!this.state.item) {
			return null;
		}
		let props = {...this.props.children.props};
		if (this.props.scope) {
			props[this.props.scope] = this.state.item;
		} else {
			props = {...props, ...this.state.item}
		}

		return React.cloneElement(this.props.children, props)
	}
}

export default ItemLoader;