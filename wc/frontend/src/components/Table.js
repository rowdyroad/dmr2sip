import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'

const TableHeader = (props) => (
	<thead>
		<tr>
			{props.columns.map((title, index) => { return <th key={index}>{title}</th>})}
		</tr>
	</thead>
)


export const TableColumn = (props) => (
	<td key={name} style={props.style} className={props.className}>
		{props.children ?
			typeof(props.children) === 'function' ? props.children(props) : props.children
			:
			props.value
		}
	</td>
)

export class Table extends Component
{
	render = () => {
		let titles = this.props.children.map((child) => { return child.props.title ? child.props.title : ''; });
		return (
				<div className="card-box">
					<div className="table-responsive">
						<table className={"table " + this.props.className}>
							<TableHeader columns={titles}/>
							<tbody>
							{
								this.props.data.map((row, index) => (
									<tr key={index}>
										{this.props.children.map((child) => (
											Object.assign({}, child, {props: {...child.props, item: row, value: row[child.props.name]}})
										))}
									</tr>
								))
							}
							</tbody>
						</table>
					</div>
				</div>
			)
	}
}

