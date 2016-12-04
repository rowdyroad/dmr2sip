import React from 'react'

const Composer = (items) => {

	return (ownProps) => {
		return items.reduceRight((prev, item) => {
			return React.createElement(item, !prev ? ownProps : {}, prev ? prev : ownProps.children);
		}, null);
	}
}


export default Composer;